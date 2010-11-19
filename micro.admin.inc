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

    $info = entity_get_info();
    dsm($info);
  foreach ($names as $key => $name) {
    $type = $types[$key];
    if (micro_hook($type->machine_name, 'form')) {
      $type_url_str = str_replace('_', '-', $type->machine_name);
      $row = array(theme('micro_admin_overview', array('name' => $name, 'machine_name' => $type)));
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
  $type = $variables['machine_name'];
  $output = check_plain($name);
  $output .= ' <small> (Machine name: ' . check_plain($type->machine_name) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($type->description) . '</div>';
  return $output;
}

/**
 * Generates the micro type editing form.
 */
function micro_type_form($form, &$form_state, $type = NULL) {
  if (!isset($type->mtid)) {
    // This is a new type, managed by micro module.
    $type = micro_type_set_defaults(array('module' => 'micro'));
  }

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

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $type->machine_name,
    '#maxlength' => 32,
    '#disabled' => !isset($type->machine_name),
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
      foreach ($data['view modes'] as $mode=>$info) {
       $entities[$entity]['modes'][$mode] = $info['label'];
      }
    }
  }

  $selected = isset($type->entity) ? $type->entity : (isset($form_state['values']['entity']) ? $form_state['values']['entity'] : key($options_entity));
  $form['entity'] = array(
    '#type' => 'select',
    '#title' => t('Entity'),
    '#options' => $options_entity,
    '#default_value' => $selected,
    '#disabled' => isset($type->entity),
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
    '#options' => _micro_get_bundle_options($entities, isset($type->entity) ? $type->entity : (isset($form_state['values']['entity']) ? $form_state['values']['entity'] : $selected)),
    '#default_value' => isset($type->bundles) ? $type->bundles : (isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : array('site')),
  );

  $form['format'] = array(
    '#title' => t("Display types"),
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#description' => t('How the micro form and items will be displayed on the attached entity.'),
  );
  //TODO - check type if set (after first submit)
  //TODO - force NOT simple page if other than site
  if ((isset($type->entity) && $type->entity != 'site') || (isset($form_state['values']['entity']) && $form_state['values']['entity'] != 'site')) {
    foreach ($entities[isset($type->entity) ? $type->entity : $form_state['values']['entity']]['modes'] as $mode=>$name) {
      $form['format'][$mode] = array(
        '#title' => t('Format for view mode "@name"', array('@name' => $name)),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('How the micro form and items will be displayed on the attached entity.'),
      );
      $form['format'][$mode]['display_' . $mode] = array(
        '#type' => 'select',
        '#title' => t('Display type'),
        '#options' => _micro_get_display_options($entities, $selected, isset($type->machine_name) ? $type->machine_name : (isset($form_state['values']['machine_name']) ? $form['values']['machine_name'] : null)),
        '#default_value' => isset($type->data) ? $type->data['format']['display_' . $mode] : (isset($form_state['values']['display']) ? $form_state['values']['display'] : 'simple_page_display'),
      );
      //TODO - check type if set (after first submit)
      //TODO - force NOT simple page if other than site
      $form['format'][$mode]['input_' . $mode] = array(
        '#type' => 'select',
        '#title' => t('Input type'),
        '#options' => _micro_get_input_options($entities, $selected, isset($type->machine_name) ? $type->machine_name : (isset($form_state['values']['machine_name']) ? $form['values']['machine_name'] : null)),
        '#default_value' => isset($type->data) ? $type->data['format']['input_' . $mode] : (isset($form_state['values']['input']) ? $form_state['values']['input'] : 'simple_page_input'),
      );
    }
  }
  else {
    $form['format']['display'] = array(
      '#type' => 'select',
      '#title' => t('Display type'),
      '#options' => array('simple_page_display' => t('Simple page display')),
      '#default_value' => 'simple_page_display',
      '#disabled' => TRUE,
    );
    //TODO - check type if set (after first submit)
    //TODO - force NOT simple page if other than site
    $form['format']['input'] = array(
      '#type' => 'select',
      '#title' => t('Input type'),
      '#options' => array('simple_page_input' => t('Simple page input')),
      '#default_value' => 'simple_page_input',
      '#disabled' => TRUE,
    );
  }


  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    /*
     *'#attached' => array(
     *  'js' => array(drupal_get_path('module', 'micro') . '/micro_types.js'),
     *),
     */
  );

  $form['workflow'] = array(
    '#type' => 'fieldset',
    '#title' => t('Workflow settings'),
    '#collapsible' => TRUE,
    '#group' => 'additional_settings',
  );
  // Disable if 'site'
  $form['workflow']['view_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('View page'),
    '#description' => t('Whether items from this micro type will have their own view page (uri). If not, they will redirect to their parent entity. Micro items attached to the site will always have a view page.'),
    '#options' => array('0' => FALSE, '1' => TRUE),
    /*
     *'#disabled' => (isset($form_state['values']['entity']) && $form_state['values']['entity'] == 'site') ? TRUE : FALSE,
     */
    '#default_value' => TRUE,
  );
  $form['workflow']['delete_confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete confirmation'),
    '#description' => t('Whether items from this micro type have a \'delete confirmation\' page.'),
    '#options' => array('0' => FALSE, '1' => TRUE),
    '#default_value' => isset($type->data['delete_confirm']) ? $type->data['delete_confirm'] : FALSE,
  );

  $form['expiration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Expiration'),
    '#collapsible' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['expiration']['expiration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Expiration'),
    '#description' => t('Whether items from this micro type will have an expiration date.'),
    '#options' => array('0' => FALSE, '1' => TRUE),
    '#default_value' => isset($type->data['expiration']) ? $type->data['expiration'] : FALSE,
  );
  $form['expiration']['expiration_time'] = array(
    '#title' => t('Expiration time'),
    '#type' => 'textfield',
    '#default_value' => isset($type->data['expiration_time']) ? $type->data['expiration_time'] : '',
    '#description' => t('The time (in seconds) after which the items will expire.'),
    '#size' => 30,
  );

  $form['base'] = array(
    '#type' => 'value',
    '#value' => $type->base,
  );
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
    if (!empty($type->mtid)) {
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
  return array($form['bundles'], $form['format']);
}

/**
 * Helper function to populate the bundle checkboxes
 *
 * @param key. This will determine which set of options is returned.
 *
 * @return array of options
 */
function _micro_get_bundle_options($entities, $selected) {
  $options = array();
  $selected == 'site' ? $options = array('site' => t('Site')) : $options = $entities[$selected]['bundles'];
  return $options;
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
  $options['none'] = 'None';
  $displays = micro_display_types();
  foreach ($displays as $key=>$value) {
    if (empty($value['micro_types']) || in_array($type, $value['micro_types'])) {
      $options[$key] = $value['label'];
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
  $options['none'] = 'None';
  $inputs = micro_input_types();
  foreach ($inputs as $key=>$value) {
    if (empty($value['micro_types']) || in_array($type, $value['micro_types'])) {
      $options[$key] = $value['label'];
    }
  }
  return $options;
}
/**
 * Validates the micro type submission form generated by micro_type_form().
 */
function micro_type_form_validate($form, &$form_state) {
  $type = new stdClass();
  $type->machine_name = trim($form_state['values']['machine_name']);
  $type->name = trim($form_state['values']['name']);
  $types = micro_type_get_names();

  // 'theme' conflicts with theme_micro_form().
  // '0' is invalid, since elsewhere we check it using empty().
  if (in_array($type->machine_name, array('0', 'theme'))) {
    form_set_error('machine_name', t("Invalid machine-readable name. Enter a name other than %invalid.", array('%invalid' => $type->machine_name)));
  }

  $names = array_flip($types);

  //TODO - validate whether there is an existing name
  if (isset($names[$type->name]) && $names[$type->name] != $old_type) {
    form_set_error('name', t('The human-readable name %name is already taken.', array('%name' => $type->name)));
  }

  if (isset($form_state['expiration']) && !isset($form_state['expiration_time'])) {
    form_set_error('expiration_time', t('You must specify an expiration time.'));
  }
}

/**
 * Implements hook_form_submit().
 */
function micro_type_form_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $type = micro_type_set_defaults();

  $type->machine_name = trim($form_state['values']['machine_name']);
  $type->name = trim($form_state['values']['name']);
  $type->description = $form_state['values']['description'];
  $type->entity = $form_state['values']['entity'];
  foreach ($form_state['values']['bundles'] as $key => $value) {
    if (empty($value)) {
      unset($form_state['values']['bundles'][$key]);
    }
  }
  $type->bundles = $form_state['values']['bundles'];
  foreach ($form_state['values'] as $key=>$value) {
    if (substr($key, 0, 5) == 'input' || substr($key, 0, 7) == 'display') {
      $data['format'][$key] = $value;
    }
  }
  // TODO - force this in form
  if ($form_state['values']['entity'] == 'site') {
    $data['view_page'] = TRUE;
  }
  else {
    $data['view_page'] = $form_state['values']['view_page'];
  }
  $data['expiration'] = $form_state['values']['expiration'];
  $data['expiration_time'] = $form_state['values']['expiration_time'];
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
    $variable = $key . '_' . $type->machine_name;

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
  if (isset($info_array[$type->machine_name])) {
    $info_array[$type->machine_name]['machine_name'] = $type->machine_name;
    $info = micro_type_set_defaults($info_array[$type->machine_name]);

    foreach ($info as $field => $value) {
      $type->$field = $value;
    }
  }
}

/**
 * Menu callback; delete a single micro type.
 */
function micro_type_delete_confirm($form, &$form_state, $type) {
  $form['machine_name'] = array('#type' => 'value', '#value' => $type->machine_name);
  $form['name'] = array('#type' => 'value', '#value' => $type->name);

  $message = t('Are you sure you want to delete the micro type %type?', array('%type' => $type->name));
  $caption = '';

  $num_micros = db_query("SELECT COUNT(*) FROM {micro} WHERE mtid = :id", array(':id' => $type->mtid))->fetchField();
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
  micro_type_delete($form_state['values']['machine_name']);

  $t_args = array('%name' => $form_state['values']['name']);
  drupal_set_message(t('The micro type %name has been deleted.', $t_args));
  watchdog('menu', 'Deleted micro type %name.', $t_args, WATCHDOG_NOTICE);

  micro_types_rebuild();
  menu_rebuild();

  $form_state['redirect'] = 'admin/structure/micro';
  return;
}
