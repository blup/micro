<?php
// $Id$

/**
 * @file
 * Get a group from a viewed page.
 */

/**
 * Implements hook_og_context_negotiation_info().
 */
function micro_og_context_og_context_negotiation_info() {
  $providers = array();

  $providers['micro'] = array(
    'name' => t('micro'),
    'description' => t("Determine context by checking if a micro is a group or a group content."),
    'callback' => 'micro_og_context_handler_micro',
    'menu path' => array('micro/%', 'micro/%/edit'),
  );

  return $providers;
}

/**
 * Context handler; Get groups from existing micro.
 */
function micro_og_context_handler_micro() {
  $return = array();
  // menu_get_object() doesn't play nicely if the path is micro/%/foo, so we load
  // it ourself.
  $item = menu_get_item();
  if (!empty($item['map'][1])) {
    if (is_object($item['map'][1])) {
      $micro = $item['map'][1];
    }
    else {
      $micro = micro_load($item['map'][1]);
    }
    $return = _group_context_handler_entity('micro', $micro);
  }
  return $return;
}

