<?php
// $Id: micro.admin.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Micro type editing UI.
 */


/**
 * Displays the micro type admin overview page.
 */
function micro_overview_types() {
  $types = micro_type_get_types();
  $names = micro_type_get_names();
  $field_ui = module_exists('field_ui');
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();

  foreach ($names as $key => $name) {
    $type = $types[$key];
    if (micro_hook($type->type, 'form')) {
      $type_url_str = str_replace('_', '-', $type->type);
      $row = array(theme('micro_admin_overview', array('name' => $name, 'type' => $type)));
      // Set the edit column.
      $row[] = array('data' => l(t('edit'), 'admin/structure/micro/manage/' . $type_url_str));

      if ($field_ui) {
        // Manage fields.
        $row[] = array('data' => l(t('manage fields'), 'admin/structure/micro/manage/' . $type_url_str . '/fields'));

        // Display fields.
        $row[] = array('data' => l(t('manage display'), 'admin/structure/micro/manage/' . $type_url_str . '/display'));
      }

      // Set the delete column.
      if ($type->module == 'micro') {
        $row[] = array('data' => l(t('delete'), 'admin/structure/micro/manage/' . $type_url_str . '/delete'));
      }
      else {
        $row[] = array('data' => '');
      }

      $rows[] = $row;
    }
  }

  $build['micro_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No micro types available. <a href="@link">Add micro type</a>.', array('@link' => url('admin/structure/micro/add'))),
  );

  return $build;
}

/**
 * Returns HTML for a micro type description for the micro type admin overview page.
 *
 * @param $variables
 *   An associative array containing:
 *   - name: The human-readable name of the micro type.
 *   - type: An object containing the 'type' (machine name) and 'description' of
 *     the micro type.
 *
 * @ingroup themeable
 */
function theme_micro_admin_overview($variables) {
  $name = $variables['name'];
  $type = $variables['type'];
  $output = check_plain($name);
  $output .= ' <small> (Machine name: ' . check_plain($type->type) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($type->description) . '</div>';
  return $output;
}

/**
 * Generates the micro type editing form.
 */
function micro_type_form($form, &$form_state, $type = NULL) {
  if (!isset($type->type)) {
    // This is a new type, managed by micro module.
    $type = micro_type_set_defaults(array('module' => 'micro'));
  }

  /*
   *foreach (entity_get_info() as $entity_type => $entity_info) {
   *  dsm($entity_type);
   *  if ($entity_info['fieldable']) {
   *    dsm($entity_info);
   *    foreach ($entity_info['bundles'] as $bundle_name => $bundle_info) {
   *      dsm($bundle_info);
   *      if (isset($bundle_info['admin'])) {
   *        dsm($bundle_info['admin']);
   *      }
   *    }
   *  }
   *}
   */
  // Make the type object available to implementations of hook_form_alter.
  $form['#micro_type'] = $type;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $type->name,
    '#description' => t('The human-readable name of this micro type. This text will be displayed as part of the list on the <em>Add new micro</em> page. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => $type->type,
    '#maxlength' => 32,
    '#disabled' => !isset($type->type),
    '#machine_name' => array(
      'exists' => 'micro_type_load',
    ),
    '#description' => t('A unique machine-readable name for this micro type. It must only contain lowercase letters, numbers, and underscores. This name will be used for constructing the URL of the %micro-add page, in which underscores will be converted into hyphens.', array(
      '%micro-add' => t('Add new micro'),
    )),
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $type->description,
    '#description' => t('Describe this micro type. The text will be displayed on the <em>Add new micro</em> page.'),
  );

  // Get all fieldable entities and their bundles.
  $options_entity['site'] = t('Site');
  foreach (entity_get_info() as $entity=>$data) {
    if ($data['fieldable']) {
      $entities[$entity] = array('label' => $data['label']);
      $options_entity[$entity] = $data['label'];
      foreach ($data['bundles'] as $bundle=>$info) {
       $entities[$entity]['bundles'][$bundle] = $info['label'];
      }
    }
  }

  $selected = isset($form_state['values']['entity']) ? $form_state['values']['entity'] : key($options_entity);
  $form['entity'] = array(
    '#type' => 'select',
    '#title' => t('Entity'),
    '#options' => $options_entity,
    '#default_value' => $selected,
    '#ajax' => array(
      'callback' => 'micro_dependent_dropdown_callback',
      'wrapper' => 'ajax-replace',
    ),
  );
  //TODO - set error if no bundle / hide if site / if reload, why does second have wrong values?
  $form['bundles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Bundles'),
    '#prefix' => '<div id="ajax-replace">',
    '#options' => _micro_get_bundle_options($entities, $selected),
    '#default_value' => isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : '',
  );
  //TODO - check type if set (after first submit)
  //TODO - force NOT simple page if other than site
  //TODO - one for each view mode
  $form['display'] = array(
    '#type' => 'select',
    '#title' => t('Display type'),
    '#options' => _micro_get_display_options($entities, $selected, $form_state['values']['type']),
    '#default_value' => isset($form_state['values']['display']) ? $form_state['values']['display'] : 'simple_page_display',
  );
  //TODO - check type if set (after first submit)
  //TODO - force NOT simple page if other than site
  //TODO - one for each view mode
  $form['input'] = array(
    '#type' => 'select',
    '#title' => t('Input type'),
    '#suffix' => '</div>',
    '#options' => _micro_get_input_options($entities, $selected, $form_state['values']['type']),
    '#default_value' => isset($form_state['values']['input']) ? $form_state['values']['input'] : 'simple_page_input',
  );

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'micro') . '/micro_types.js'),
    ),
  );

  $form['submission'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submission form settings'),
    '#collapsible' => TRUE,
    '#group' => 'additional_settings',
  );
  /*
   *$form['submission']['title_label'] = array(
   *  '#title' => t('Title field label'),
   *  '#type' => 'textfield',
   *  '#default_value' => $type->title_label,
   *  '#required' => TRUE,
   *);
   */
  $form['submission']['help']  = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#default_value' => $type->help,
    '#description' => t('This text will be displayed at the top of the page when creating or editing content of this type.'),
  );
  /*
   *$form['workflow'] = array(
   *  '#type' => 'fieldset',
   *  '#title' => t('Publishing options'),
   *  '#collapsible' => TRUE,
   *  '#collapsed' => TRUE,
   *  '#group' => 'additional_settings',
   *);
   *$form['workflow']['micro_options'] = array('#type' => 'checkboxes',
   *  '#title' => t('Default options'),
   *  '#default_value' => variable_get('micro_options_' . $type->type, array('status', 'promote')),
   *  '#options' => array(
   *    'status' => t('Published'),
   *    'promote' => t('Promoted to front page'),
   *    'sticky' => t('Sticky at top of lists'),
   *    'revision' => t('Create new revision'),
   *  ),
   *  '#description' => t('Users with the <em>Administer micro</em> permission will be able to override these options.'),
   *);
   *$form['display'] = array(
   *  '#type' => 'fieldset',
   *  '#title' => t('Display settings'),
   *  '#collapsible' => TRUE,
   *  '#collapsed' => TRUE,
   *  '#group' => 'additional_settings',
   *);
   *$form['display']['micro_submitted'] = array(
   *  '#type' => 'checkbox',
   *  '#title' => t('Display author and date information.'),
   *  '#default_value' => variable_get('micro_submitted_' . $type->type, TRUE),
   *  '#description' => t('Author username and publish date will be displayed.'),
   *);
   */
  $form['base'] = array(
    '#type' => 'value',
    '#value' => $type->base,
  );
  /*
   *$form['custom'] = array(
   *  '#type' => 'value',
   *  '#value' => $type->custom,
   *);
   */
  /*
   *$form['modified'] = array(
   *  '#type' => 'value',
   *  '#value' => $type->modified,
   *);
   */
  /*
   *$form['locked'] = array(
   *  '#type' => 'value',
   *  '#value' => $type->locked,
   *);
   */

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save micro type'),
    '#weight' => 40,
  );

  if ($type->module == 'micro') {
    if (!empty($type->type)) {
      $form['actions']['delete'] = array(
        '#type' => 'submit',
        '#value' => t('Delete micro type'),
        '#weight' => 45,
      );
    }
  }

  return $form;
}

/**
 * Selects just the second dropdown to be returned for re-rendering
 *
 * @return renderable array
 */
function micro_dependent_dropdown_callback($form, $form_state) {
  return array($form['bundles'], $form['display'], $form['input']);
}

/**
 * Helper function to populate the bundle checkboxes
 *
 * @param key. This will determine which set of options is returned.
 *
 * @return array of options
 */
function _micro_get_bundle_options($entities, $selected) {
  $selected == 'site' ? $options = array('site' => t('Site')) : $options = $entities[$selected]['bundles'];
  if (isset($options)) {
    return $options;
  }
  else {
    return array();
  }
}

/**
 * Helper function to populate the display options
 *
 * @param key. This will determine which set of options is returned.
 *
 * @return array of options
 */
function _micro_get_display_options($entities, $selected, $type) {
  $options = array();
  if ($selected == 'site') {
    $options = array('simple_page_display' => t('Simple page display'));
  }
  else {
    $displays = micro_display_types();
    foreach ($displays as $key=>$value) {
      if (empty($value['micro_types']) || in_array($type, $value['micro_types'])) {
        $options[$key] = $value['label'];
      }
    }
  }
  return $options;
}

/**
 * Helper function to populate the input options
 *
 * @param key. This will determine which set of options is returned.
 *
 * @return array of options
 */
function _micro_get_input_options($entities, $selected, $type) {
  $options = array();
  if ($selected == 'site') {
    $options = array('simple_page_input' => t('Simple page input'));
  }
  else {
    $inputs = micro_input_types();
    foreach ($inputs as $input) {
      if (empty($input['micro_types']) || in_array($type, $input['micro_types'])) {
        $options[key($inputs)] = $input['label'];
      }
      next($inputs);
    }
  }
  return $options;
}
/**
 * Validates the micro type submission form generated by micro_type_form().
 */
function micro_type_form_validate($form, &$form_state) {
  $type = new stdClass();
  $type->type = trim($form_state['values']['type']);
  $type->name = trim($form_state['values']['name']);
  $types = micro_type_get_names();

  // 'theme' conflicts with theme_micro_form().
  // '0' is invalid, since elsewhere we check it using empty().
  if (in_array($type->type, array('0', 'theme'))) {
    form_set_error('type', t("Invalid machine-readable name. Enter a name other than %invalid.", array('%invalid' => $type->type)));
  }

  $names = array_flip($types);

  //TODO - validate whether there is an existing name
  if (isset($names[$type->name]) && $names[$type->name] != $old_type) {
    form_set_error('name', t('The human-readable name %name is already taken.', array('%name' => $type->name)));
  }
}

/**
 * Implements hook_form_submit().
 */
function micro_type_form_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $type = micro_type_set_defaults();

  $type->type = trim($form_state['values']['type']);
  $type->name = trim($form_state['values']['name']);
  $type->description = $form_state['values']['description'];
  $type->entity = $form_state['values']['entity'];
  foreach ($form_state['values']['bundles'] as $key => $value) {
    if (empty($value)) {
      unset($form_state['values']['bundles'][$key]);
    }
  }
  $type->bundles = $form_state['values']['bundles'];
  $data['input'] = $form_state['values']['input'];
  $data['display'] = $form_state['values']['display'];
  $type->data = $data;

  $type->base = !empty($form_state['values']['base']) ? $form_state['values']['base'] : 'micro_content';
  if (isset($form['#micro_type']->module)) {
    $type->module = $form['#micro_type']->module;
  }

  if ($op == t('Delete micro type')) {
    $form_state['redirect'] = 'admin/structure/micro/manage/' . str_replace('_', '-', $type->old_type) . '/delete';
    return;
  }

  $variables = $form_state['values'];

  // Remove everything that's been saved already - whatever's left is assumed
  // to be a persistent variable.
  foreach ($variables as $key => $value) {
    if (isset($type->$key)) {
      unset($variables[$key]);
    }
  }

  unset($variables['form_token'], $variables['op'], $variables['submit'], $variables['delete'], $variables['reset'], $variables['form_id'], $variables['form_build_id']);

  // Save or reset persistent variable values.
  foreach ($variables as $key => $value) {
    $variable = $key . '_' . $type->type;

    if (is_array($value)) {
      $value = array_keys(array_filter($value));
    }
    variable_set($variable, $value);
  }

  // Saving the micro type after saving the variables allows modules to act
  // on those variables via hook_micro_type_insert().
  $status = micro_type_save($type);

  micro_types_rebuild();
  menu_rebuild();
  $t_args = array('%name' => $type->name);

  if ($status == SAVED_UPDATED) {
    drupal_set_message(t('The micro type %name has been updated.', $t_args));
  }
  elseif ($status == SAVED_NEW) {
    drupal_set_message(t('The micro type %name has been added.', $t_args));
    watchdog('micro', 'Added micro type %name.', $t_args, WATCHDOG_NOTICE, l(t('view'), 'admin/structure/micro'));
  }

  $form_state['redirect'] = 'admin/structure/micro';
  return;
}

/**
 * Resets all of the relevant fields of a module-defined micro type to their
 * default values.
 *
 * @param $type
 *   The micro type to reset. The micro type is passed back by reference with its
 *   resetted values. If there is no module-defined info for this micro type,
 *   then nothing happens.
 */
function micro_type_reset($type) {
  $info_array = module_invoke_all('micro_info');
  if (isset($info_array[$type->type])) {
    $info_array[$type->type]['type'] = $type->type;
    $info = micro_type_set_defaults($info_array[$type->type]);

    foreach ($info as $field => $value) {
      $type->$field = $value;
    }
  }
}

/**
 * Menu callback; delete a single micro type.
 */
function micro_type_delete_confirm($form, &$form_state, $type) {
  $form['type'] = array('#type' => 'value', '#value' => $type->type);
  $form['name'] = array('#type' => 'value', '#value' => $type->name);

  $message = t('Are you sure you want to delete the micro type %type?', array('%type' => $type->name));
  $caption = '';

  $num_micros = db_query("SELECT COUNT(*) FROM {micro} WHERE type = :type", array(':type' => $type->type))->fetchField();
  if ($num_micros) {
    $caption .= '<p>' . format_plural($num_micros, '%type is used by 1 piece of content on your site. If you remove this micro type, you will not be able to edit the %type micro and it may not display correctly.', '%type is used by @count pieces of content on your site. If you remove %type, you will not be able to edit the %type micro and it may not display correctly.', array('%type' => $type->name)) . '</p>';
  }

  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, $message, 'admin/structure/micro', $caption, t('Delete'));
}

/**
 * Process micro type delete confirm submissions.
 */
function micro_type_delete_confirm_submit($form, &$form_state) {
  micro_type_delete($form_state['values']['type']);

  $t_args = array('%name' => $form_state['values']['name']);
  drupal_set_message(t('The micro type %name has been deleted.', $t_args));
  watchdog('menu', 'Deleted micro type %name.', $t_args, WATCHDOG_NOTICE);

  micro_types_rebuild();
  menu_rebuild();

  $form_state['redirect'] = 'admin/structure/micro';
  return;
}
