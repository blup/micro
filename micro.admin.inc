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
  $types = micro_get_types();

  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();

  foreach($types as $id => $type) {
    $type_url_str = str_replace('_', '-', $id);

    // Start building the row and add a name label and edit operation link
    $row = array();
    $row[] = array('data' => theme('micro_admin_type', array('label' => $type->label, 'type' => $type)));
    $row[] = array('data' => l(t('edit'), 'admin/structure/micro/manage/' . $type_url_str));

    // If not locked, add a delete column
    if (!$type->isLocked()) {
      $row[] = array('data' => l(t('delete'), 'admin/structure/micro/manage/' . $type_url_str . '/delete'));
    }
    else {
      $row[] = array('data' => '');
    }

    // Add this row onto the table
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No micros available. <a href="@link">Add a micro</a>.', array('@link' => url('admin/structure/micro/add'))), 'colspan' => '5', 'class' => array('message')));
  }

  $build['micro_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows
  );
  return $build;
}

/**
 * Theme the label and machine name for the micro type admin list.
 */
function theme_micro_admin_type($variables) {
  $label = $variables['label'];
  $micro_type  = $variables['type'];

  $output = check_plain($label);
  $output .= ' <small> (Machine name: ' . check_plain($micro_type->type) . ')</small>';
  // @todo if we add a description column to micro types.
  //$output .= '<div class="description">' . filter_xss_admin($type['description']) . '</div>';
  return $output;
}

/**
 * Generates the micro type editing form.
 */
function micro_type_form($form, &$form_state, $micro_type = NULL) {
  if (!isset($micro_type)) {
    // This is a new type.
    $micro_type = new MicroType();
  }

  // Make the type object available to implementations of hook_form_alter.
  $form_state += array('micro_type' => $micro_type);

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $micro_type->label,
    '#description' => t('The human-readable name of this micro type. This text will be displayed as part of the list on the <em>TO DO</em> page. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
    '#required' => TRUE,
    '#size' => 30,
    '#field_suffix' => ' <small id="edit-name-suffix">' . ($micro_type->isLocked() ? t('Machine name: @name', array('@name' => $micro_type->type)) : '&nbsp;') . '</small>',
  );

  // Machine-readable type
  $form['type'] = array(
    '#title' => t('Type'),
    '#type' => 'textfield',
    '#description' => t('Computer-readable type. Use only letters and numbers. This must also be unique.'),
    '#required' => TRUE,
    '#size' => 20,
    '#default_value' => $micro_type->type,
  );

  // If the micro is locked, we don't want people seeing or editing the
  // machine-readable type: no point
  if ($micro_type->isLocked()) {
    $form['type']['#type'] = 'value';
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save micro type'),
    '#weight' => 40,
  );

  if (!$micro_type->isLocked()) {
    if (!empty($micro_type->type)) {
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
 * Form API submit callback for micro_type_form
 */
function micro_type_form_submit(&$form, &$form_state) {
  // Grab micro_type back out of the $form array
  $micro_type = $form_state['micro_type'];

  // Set the name and, if it's not set (and the type isn't locked)
  // also set the machine-readable micro type
  $micro_type->label = $form_state['values']['name'];
  if (!$micro_type->isLocked() && empty($micro_type->type)) {
    $micro_type->type = $form_state['values']['type'];
  }

  // Save and go back
  $micro_type->save();
  $form_state['redirect'] = 'admin/structure/micro';
}

