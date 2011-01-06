<?php
// $Id$

/**
 * @file
 * Group UI.
 */

/**
 * Implement hook_form_FORM_ID_alter().
 */
function og_form_micro_type_form_alter(&$form, &$form_state) {
  $micro_type = $form['#micro_type']->machine_name;
  $form['group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Group'),
    '#collapsible' => TRUE,
    '#group' => 'additional_settings',
    '#description' => t('Specify how Group should treat content of this type. Content may behave as a group, as group content, or may not participate in Group at all.'),
  );

  $form['group']['og_group_type'] = array(
    '#type' => 'radios',
    '#title' => t('Group'),
    '#default_value' =>  og_is_group_type('micro', $micro_type) ? 'group' : 'omitted',
    '#options' => array('omitted' => t('Not a group type'), 'group' => t('Group type')),
  );

  $form['group']['og_group_content_type'] = array(
    '#type' => 'radios',
    '#title' => t('Group content'),
    '#default_value' =>  og_is_group_content_type('micro', $micro_type) ? 'og_content' : 'omitted',
    '#options' => array('omitted' => t('Not a group content type'), 'og_content' => t('Group content type')),
  );

  $form['#validate'][] = 'micro_og_micro_type_validate';
}

function micro_og_micro_type_validate(&$form, &$form_state) {
  $bundles = array_filter($form_state['values']['bundles']);
  $og = $form_state['values']['og_group_type'];
  $og_content = $form_state['values']['og_group_content_type'];
  foreach ($bundles as $bundle) {
    // If the parent bundle is a group, this micro type has to be a group content type.
    if (og_is_group_type('micro', $bundle)) {
      if($og_content == 'ommited') {
        form_set_error('group][og_group_content_type', t("The parent bundle @bundle is a group type. Please make this a group content type.", array('@bundle' => $bundle)));
      }
    }
    // If the parent bundle is a group content type, the micro type cannot be a group nor group content type.
    if (og_is_group_content_type('micro', $bundle)) {
      if ($og == 'group' || $og_content == 'og_content') {
        form_set_error('group', t("The parent bundle @bundle is a group content type. This micro type cannot be a group nor group content type.", array('@bundle' => $bundle)));
      }
    }
  }
  // If the micro type is a group, force the view page option.
  if ($og == 'group') {
    $form_state['values']['view_page'] = TRUE;
  }
  // If the micro type is a group content type, check that its parent is a group type.
  if ($og_content == 'og_content') {
    foreach ($bundles as $bundle) {
      if (!og_is_group_type($bundle)) {
        form_set_error('bundles', t("This micro is a group content type. The parent bundle @bundle must be a group.", array('@bundle' => $bundle)));
      }
    }
  }
}


/**
 * Implement hook_form_FORM_ID_alter().
 */
function micro_form_node_type_alter(&$form, &$form_state) {
  $form['#submit'] = 'micro_og_node_type_submit';
}

function micro_og_node_type_submit(&$form, &$form_state) {
  $bundles = micro_types_from_entity($form_state['#node'], $form_state['#node']->type);
  $og = $form_state['values']['og_group_type'];
  $og_content = $form_state['values']['og_content_type'];
  if ($og = 'group') {
    foreach ($bundles as $id => $values) {
      if (!og_is_group_content_type($values)) {
        drupal_set_message(t("The micro type @bundle is no longer the node @type's child since it's not a group content type.", array('@bundle' => $values->name, '@type' => $form_state['#node']->type)), 'warning');
        micro_type_desactivate_bundle($id, $form_state['#node']->type);
      }
    }
  }
  if ($og_content = 'og_content') {
    foreach ($bundles as $id => $values) {
      if (og_is_group_content_type($values) || og_is_group($values)) {
        drupal_set_message(t("The micro type @bundle is no longer the node @type's child since it a group or group content type.", array('@bundle' => $values->name, '@type' => $form_state['#node']->type)), 'warning');
        micro_type_desactivate_bundle($id, $form_state['#node']->type);
      }
    }
  }
}

