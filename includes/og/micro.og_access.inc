<?php
// $Id$

/**
 * @file
 * Enable access control for private and public groups and group content.
 */

/**
 * Implementation of hook_micro_grants().
 */
function og_access_micro_grants($account, $op) {
  $grants = array();
  if ($op == 'view' && $groups = og_get_entity_groups('user', $account)) {
    foreach ($groups as $gid => $value) {
      // Not all the groups are micros, but in order to save loading the groups
      // we grant all group IDs.
      $grants[OG_ACCESS_AUTHENTICATED_REALM][] = $gid;
    }
  }

  return $grants;
}

/**
 * Implementation of hook_micro_access_records.
 */
function og_access_micro_access_records($micro) {
  $grants = array();

  // The group IDs, that in case access is granted, will be recorded.
  $gids = array();

  $access = FALSE;
  if (!empty($micro->{OG_ACCESS_FIELD}[LANGUAGE_NONE][0]['value'])) {
    // Group or group content that is explicitly set to be unpublic.
    $access = TRUE;
    $group = og_get_group('micro', $micro->mid);
    $gids[] = $group->gid;
  }
  elseif (isset($micro->{OG_CONTENT_ACCESS_FIELD}[LANGUAGE_NONE][0]['value'])) {
    switch ($micro->{OG_CONTENT_ACCESS_FIELD}[LANGUAGE_NONE][0]['value']) {
      case OG_CONTENT_ACCESS_DEFAULT:
        if ($field = field_info_field(OG_ACCESS_FIELD)) {
          // Access should be determined by its groups. If group content belongs
          // to several groups, and one of them is private, then the group
          // content will private as-well.
          $gids = og_get_entity_groups('micro', $micro);

          $groups = og_load_multiple($gids);

          // Get all groups under their entity.
          $list = array();
          foreach ($groups as $group) {
            $list[$group->entity_type][$group->etid] = $group->etid;
          }

          // If group content belongs to several groups, and one of them is
          // private, then this variable decides what should happen -- if the
          // group content will be private as-well or become public.
          // By default, if one group is private then the group content will be
          // private.
          $strict_private = variable_get('group_access_strict_private', 1);

          $total_count = 0;

          foreach ($list as $entity_type => $entity_gids) {
            $query = new EntityFieldQuery;
            $count = $query
              ->entityCondition('entity_type', $entity_type)
              ->entityCondition('entity_id', $entity_gids, 'IN')
              ->fieldCondition(OG_ACCESS_FIELD, 'value', 1, '=')
              ->count()
              ->execute();

            if ($strict_private) {
              // There is at least one private group and 'strict private' is
              // TRUE so this group content should be private.
              if ($count) {
                $access = TRUE;
                break;
              }
            }
            else {
              // 'strict private' is FALSE so count all the groups, and only if
              // all of them are private then this group content should be
              // private.
              $total_count += $count;
            }
          }
          if ($total_count == count($gids)) {
            // All groups are private.
            $access = TRUE;
          }
        }
        break;

      case OG_CONTENT_ACCESS_PUBLIC:
        // Do nothing.
        break;

      case OG_CONTENT_ACCESS_PRIVATE:
        $access = TRUE;
        $gids = og_get_entity_groups('micro', $micro);
        break;

    }
  }

  //TODO: Add a case where the group has access field but group content doesn't.

  if ($access && $gids) {
    foreach ($gids as $gid) {
      $grants[] = array (
        'realm' => OG_ACCESS_AUTHENTICATED_REALM,
        'gid' => $gid,
        'grant_view' => 1,
        'grant_update' => 0,
        'grant_delete' => 0,
        'priority' => 0,
      );
    }
  }

  return $grants;
}

/**
 * Implement hook_og_fields_info().
 */
function og_access_og_fields_info() {
  $allowed_values = array(
    '0|Public - accessible to all site users',
    '1|Private - accessible only to group members',
  );
  $items[OG_ACCESS_FIELD] = array(
    'type' => array('group'),
    'description' => t('Add Group access field to group types.'),
    // Private access can be done only on micro entity.
    'entity' => array('micro'),
    'field' => array(
      'field_name' => OG_ACCESS_FIELD,
      'no_ui' => TRUE,
      'type' => 'list_boolean',
      'cardinality' => 1,
      'settings' => array('allowed_values' => implode("\r\n", $allowed_values), 'allowed_values_function' => ''),
    ),
    'instance' => array(
      'label' => t('Group visibility'),
      'required' => TRUE,
      // Default to public.
      'default_value' => array(0 => array('value' => 0)),
      'widget_type' => 'options_select',
      'view modes' => array(
        'full' => array(
          'label' => 'above',
          'type' => 'options_onoff',
        ),
        'teaser' => array(
          'label' => 'above',
          'type' => 'options_onoff',
        ),
      ),
    ),
  );

  $allowed_values = array(
    '0|Use group defaults',
    '1|Public - accessible to all site users',
    '2|Private - accessible only to group members',

  );
  $items[OG_CONTENT_ACCESS_FIELD] = array(
    'type' => array('group content'),
    'description' => t('Add Group access field to group-content types, that may override the group settings.'),
    // Private access can be done only on micro entity.
    'entity' => array('micro'),
    'field' => array(
      'field_name' => OG_CONTENT_ACCESS_FIELD,
      'no_ui' => TRUE,
      'type' => 'list_number',
      'cardinality' => 1,
      'settings' => array('allowed_values' => implode("\r\n", $allowed_values), 'allowed_values_function' => ''),
    ),
    'instance' => array(
      'label' => t('Group content visibility'),
      'required' => TRUE,
      // Make the group type default.
      'default_value' => array(0 => array('value' => 0)),
      'widget_type' => 'options_select',
      'view modes' => array(
        'full' => array(
          'label' => 'above',
          'type' => 'list_default',
        ),
        'teaser' => array(
          'label' => 'above',
          'type' => 'list_default',
        ),
      ),
    ),
  );

  return $items;
}

