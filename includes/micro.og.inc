<?php
// $Id$

/**
 * @file
 * Organic Group integration for Micro module.
 */

include_once(drupal_get_path('module', 'micro') . '/includes/og/micro.og_micro_link.inc');
if (module_exists('og_context')) {
  include_once(drupal_get_path('module', 'micro') . '/includes/og/micro.og_context.inc');
}
if (module_exists('og_ui')) {
  include_once(drupal_get_path('module', 'micro') . '/includes/og/micro.og_ui.inc');
}

/**
 * Implements hook_og_views_relationship().
 *
 * Add relationship information for all core's entities.
 */
function micro_og_views_relationship() {
  $items = array();

  $items['micro'] = array(
    'entity' => 'micro',
    'views table' => 'micro',
    'join field' => 'mid',
  );

  return $items;
}

/**
 * Implement hook_og_permission().
 */
function micro_og_permission() {
  // Generate standard micro permissions for all applicable micro types.
  $perms = array();
  foreach (micro_permissions_get_configured_types() as $type) {
    $perms = array_merge($perms, micro_list_permissions($type));
  }

  return $perms;
}

/**
 * Implement hook_micro_access()
 */
function og_micro_access($micro, $op, $account) {
  // If not a group type or the operation is micro creation which still has no
  // groups so we can't check it yet, we ignore the access.
  $return = MICRO_ACCESS_IGNORE;

  $type = is_string($micro) ? $micro : (is_array($micro) ? $micro['type'] : $micro->type);
  if (in_array($op, array('update', 'delete'))) {
    if (og_is_group_type('micro', $type) && $group = og_get_group('micro', $micro->mid)) {
      $return = MICRO_ACCESS_DENY;
      // The micro is a group.
      if (og_user_access($group->gid, $op . ' group', $account)) {
        $return = MICRO_ACCESS_ALLOW;
      }
    }
    // A single content type can act as a group and as group content, so make
    // sure that even if the user doesn't have access on the group itself,
    // further access checks are performed.
    // However if we already have access, then skip the following access checks.
    if ($return != MICRO_ACCESS_ALLOW && og_is_group_content_type('micro', $type) && $gids = og_get_entity_groups('micro', $micro)) {
      $return = MICRO_ACCESS_DENY;
      // We don't have a context, so we need to get all the permissions
      // of all the groups. We don't intersect with the user's group, as
      // groups might allow anonymous members access.
      foreach ($gids as $gid) {
        if (og_user_access($gid, "administer group", $account) ||
          // Any content.
          og_user_access($gid, "$op any $type content", $account) ||
          // Users own content.
          (og_user_access($gid, "$op own $type content", $account) && $micro->uid == $account->uid)) {
          $return =  MICRO_ACCESS_ALLOW;
          break;
        }
      }
    }
  }

  return $return;
}


/**
* Implement hook_micro_type_delete().
*
* We immediately delete those variables as they are only used to indicate a
* content type should be a group or a group content. However, the actual
* indication for it is in the field API. This is just a workaround, specifically
* for the micro entity, to allow users to define groups via the "micro type"
* page.
*/
function og_micro_type_delete($info) {
  variable_del('og_group_type_' . $info->machine_name);
  variable_del('og_group_content_type_' . $info->machine_name);
}

/**
* Implement hook_micro_type_insert().
*/
function og_micro_type_insert($info) {
  og_micro_type_save($info->machine_name);
}

/**
* Implement hook_micro_type_update().
*/
function og_micro_type_update($info) {
  og_micro_type_save($info->machine_name);
}

/**
 * Add group and group content fields to new content types.
 *
 * @param $bundle_name
 *   The content type name.
 */
function og_micro_type_save($bundle_name) {
  if (variable_get('og_group_type_' . $bundle_name, 'omitted') == 'group') {
    og_create_field(OG_GROUP_FIELD,'micro', $bundle_name);
    // Delete the variable, as we will rely on the presence of the field.
    variable_del('og_group_type_' . $bundle_name);
  }
  if (variable_get('og_group_content_type_' . $bundle_name, 'omitted') == 'og_content') {
    og_create_field(OG_AUDIENCE_FIELD,'micro', $bundle_name);
    // Delete the variable, as we will rely on the presence of the field.
    variable_del('og_group_content_type_' . $bundle_name);
  }
}

/**
 * Helper function to generate standard micro permission list for a given type.
 *
 * @param $type
 *   The machine-readable name of the micro type.
 * @return array
 *   An array of permission names and descriptions.
 */
function micro_og_list_permissions($type) {
  $perms = array();

  // Check type is of group content.
  if (og_is_group_content_type('micro', $type)) {
    $info = micro_type_get_type($type);
    $type = check_plain($info->type);

    // Build standard list of micro permissions for this type.
    $perms = array(
      "update own $type content" => array(
        'title' => t('Edit own %type_name content', array('%type_name' => $info->name)),
      ),
      "update any $type content" => array(
        'title' => t('Edit any %type_name content', array('%type_name' => $info->name)),
      ),
      "delete own $type content" => array(
        'title' => t('Delete own %type_name content', array('%type_name' => $info->name)),
      ),
      "delete any $type content" => array(
        'title' => t('Delete any %type_name content', array('%type_name' => $info->name)),
      ),
    );

    // Add default permissions.
    foreach ($perms as $key => $value) {
      $perms[$key]['default role'] = array(OG_AUTHENTICATED_ROLE);
    }
  }
  return $perms;
}
