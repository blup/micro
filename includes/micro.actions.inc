<?php
// $Id$

/**
 * @file
 * Actions integration.
 */

/**
 * Implements hook_action_info().
 * TODO - micro_delete_action
 * TODO - micro_delete_from_user_action
 * TODO - micro_change_eid_action
 * TODO - micro_change_user_action
 */
function micro_action_info() {
  return array(
    'micro_save_action' => array(
      'type' => 'micro',
      'label' => t('Save content'),
      'configurable' => FALSE,
      'triggers' => array('comment_insert', 'comment_update', 'comment_delete'),
    ),
  );
}

/**
 * Saves a micro.
 *
 * @ingroup actions
 */
function micro_save_action($micro) {
  micro_save($micro);
  watchdog('action', 'Saved @type %id', array('@type' => micro_type_get_name($micro), '%title' => $micro->mid));
}

