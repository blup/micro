<?php
// $Id$ micro.pages.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Page callbacks for adding, editing, and deleting micro items.
 */

/**
 * Menu callback; presents the micro editing form, or redirects to delete confirmation.
 */
function micro_page_edit($micro) {
  $type = micro_type_get_type($micro);
  drupal_set_title(t('<em>Edit @type</em> @mid', array('@type' => $type->name, '@mid' => $micro->mid)), PASS_THROUGH);
  return drupal_get_form($type->machine_name . '_micro_form', $micro);
}

function micro_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);
  // Bypass the micro/add listing if only one content type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('micro_add_list', array('content' => $content));
}

/**
 * Returns HTML for a list of available micro types for micro creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_micro_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="micro-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any micro types yet. Go to the <a href="@create-micro">micro type creation page</a> to add a new micro type.', array('@create-micro' => url('admin/structure/micro/add'))) . '</p>';
  }
  return $output;
}


/**
 * Present a micro submission form or a set of links to such forms.
 */
function micro_add($type, $id) {
  global $user;

  $types = micro_type_get_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  // If a micro type has been specified, validate its existence.
  if (isset($types[$type])) {
    // Initialize settings:
    $micro = micro_create(array('uid' => $user->uid, 'mtid' => $types[$type]->mtid, 'eid' => $id, 'type' => $type));

    drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)), PASS_THROUGH);
    $output = drupal_get_form($type . '_micro_form', $micro);
  }

  return $output;
}

function micro_form_validate($form, &$form_state) {
  // $form_state['micro'] contains the actual entity being edited, but we must
  // not update it with form values that have not yet been validated, so we
  // create a pseudo-entity to use during validation.
  $micro = (object) $form_state['values'];
  micro_validate($micro, $form, $form_state);
  entity_form_field_validate('micro', $form, $form_state);
}

/**
 * Generate the micro add/edit form array.
 */
function micro_form($form, &$form_state, $micro) {
  global $user;

  // During initial form build, add the micro entity to the form state for use
  // during form building and processing. During a rebuild, use what is in the
  // form state.
  if (!isset($form_state['micro'])) {
    micro_object_prepare($micro);
    $form_state['micro'] = $micro;
  }
  else {
    $micro = $form_state['micro'];
  }

  $micro_type = micro_type_get_type($micro);

  $form['#micro'] = $micro;
  $form['#micro_type'] = $micro_type;
  $form['#micro_edit_form'] = TRUE;
  $form['#attributes']['class'][] = 'micro-form';
  if (!empty($micro->mtid)) {
    $form['#attributes']['class'][] = 'micro-' . $micro_type->machine_name . '-form';
  }

  // Basic micro information.
  // These elements are just values so they are not even sent to the client.
  foreach (array('mid', 'uid', 'eid', 'created', 'type', 'mtid') as $key) {
    $form[$key] = array(
      '#type' => 'value',
      '#value' => isset($micro->$key) ? $micro->$key : NULL,
    );
  }

  // Invoke hook_form() to get the micro-specific bits. Can't use micro_invoke(),
  // because hook_form() needs to be able to receive $form_state by reference.
  // @todo hook_form() implementations are unable to add #validate or #submit
  //   handlers to the form buttons below. Remove hook_form() entirely.
  $function = micro_type_get_module($micro) . '_content_form';
  if (function_exists($function) && ($extra = $function($micro, $form_state))) {
    $form = array_merge_recursive($form, $extra);
  }

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('micro_form_submit'),
  );
  if (!empty($micro->mid) && micro_access('delete', $micro)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('micro_form_delete_submit'),
    );
  }
  // This form uses a button-level #submit handler for the form's main submit
  // action. micro_form_submit() manually invokes all form-level #submit handlers
  // of the form. Without explicitly setting #submit, Form API would auto-detect
  // micro_form_submit() as submit handler, but that is the button-level #submit
  // handler for the 'Save' action.
  $form['#validate'][] = 'micro_form_validate';
  if (!isset($form['#submit']) && function_exists($micro_type->machine_name . '_micro_form_submit')) {
    $form['#submit'][] = $micro_type->machine_name . '_micro_form_submit';
  }
  $form += array('#submit' => array());

  $form['#builder_function'] = 'micro_form_submit_build_micro';
  field_attach_form('micro', $micro, $form, $form_state);

  return $form;
}

/**
 * Button submit function: handle the 'Delete' button on the micro form.
 */
function micro_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $micro = $form['#micro'];
  $form_state['redirect'] = array('micro/' . $micro->mid . '/delete', array('query' => $destination));
}


function micro_form_submit($form, &$form_state) {
  $micro = $form['#builder_function']($form, $form_state);
  $micro_type = $form['#micro_type'];

  $insert = empty($micro->mid);
  micro_save($micro);
  $micro_link = l(t('view'), 'micro/' . $micro->mid);
  $watchdog_args = array('@type' => $micro_type->machine_name, '%mid' => $micro->mid);
  $t_args = array('@type' => micro_type_get_name($micro), '%mid' => $micro->mid);

  if ($insert) {
    watchdog('content', '@type: added %mid.', $watchdog_args, WATCHDOG_NOTICE, $micro_link);
    drupal_set_message(t('@type %mid has been created.', $t_args));
  }
  else {
    watchdog('content', '@type: updated %mid.', $watchdog_args, WATCHDOG_NOTICE, $micro_link);
    drupal_set_message(t('@type %mid has been updated.', $t_args));
  }
  if ($micro->mid) {
    $form_state['values']['mid'] = $micro->mid;
    $form_state['mid'] = $micro->mid;
    $form_state['redirect'] = 'micro/' . $micro->mid;
  }
  else {
    // In the unlikely case something went wrong on save, the micro will be rebuilt
    drupal_set_message(t('The post could not be saved.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
  // Clear the page and block caches.
  cache_clear_all();
}

/**
 * Updates the form state's micro entity by processing this submission's values.
 *
 * This is the default #builder_function for the micro form. It is called
 * during the "Save" submit handler to retrieve the entity to
 * save or preview. This function can also be called by a "Next" button of a
 * wizard to update the form state's entity with the current step's values
 * before proceeding to the next step.
 *
 * @see micro_form()
 */
function micro_form_submit_build_micro($form, &$form_state) {
  unset($form_state['submit_handlers']);

  form_execute_handlers('submit', $form, $form_state);

  $micro = $form_state['micro'];
  entity_form_submit_build_entity('micro', $micro, $form, $form_state);

  micro_submit($micro);
  foreach (module_implements('micro_submit') as $module) {
    $function = $module . '_micro_submit';
    $function($micro, $form, $form_state);
  }
  return $micro;
}

/**
 * Menu callback -- ask for confirmation of micro deletion
 */
function micro_delete_confirm($form, &$form_state, $micro) {
  $form['#micro'] = $micro;
  $type = micro_type_get_type($micro);
  // Always provide entity id in the same form key as in the entity edit form.
  $form['mid'] = array('#type' => 'value', '#value' => $micro->mid);
  return confirm_form($form,
    t('Are you sure you want to delete @type %mid?', array('@type' => $type->name, '%mid' => $micro->mid)),
    'micro/' . $micro->mid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute micro deletion
 */
function micro_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $micro = micro_load($form_state['values']['mid']);
    $micro_type = micro_type_get_type($micro);
    micro_delete($form_state['values']['mid']);
    watchdog('content', '@type: deleted %mid.', array('@type' => $micro_type->machine_name, '%mid' => $micro->mid));
    drupal_set_message(t('@type %mid has been deleted.', array('@type' => $micro_type->name, '%mid' => $micro->mid)));
  }

  $form_state['redirect'] = '<front>';
}

