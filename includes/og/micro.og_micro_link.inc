<?php
// $Id$

/**
 * @file
 * Add content creation links based on group context.
 */

/**
 * Group micro link field.
 */
define('OG_MICRO_LINK_FIELD', 'og_micro_link');

/**
 * Implementation of hook_views_api().
 */
function micro_og_micro_link_views_api() {
  return array(
    'api' => 2,
    'path' => drupal_get_path('module', 'micro_og_micro_link') . '/includes',
  );
}

/**
 * Implement hook_field_formatter_info().
 */
function micro_og_micro_link_field_formatter_info() {
  return array(
    'micro_og_micro_link_default' => array(
      'label' => t('Group content links'),
      'field types' => array('list_text'),
    ),
  );
}

/**
 * Implement og_fields_info().
 */
function micro_og_micro_link_og_fields_info() {
  $items[OG_MICRO_LINK_FIELD] = array(
    'type' => array('group'),
    'description' => t('Add content creation links based on group context.'),
    'field' => array(
      'field_name' => OG_MICRO_LINK_FIELD,
      'no_ui' => TRUE,
      'type' => 'list_text',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'settings' => array('allowed_values' => '', 'allowed_values_function' => 'og_micro_link_field_allowed_values'),
    ),
    'instance' => array(
      'label' => t('Group content links'),
      'widget_type' => 'options_select',
      'view modes' => array(
        'full' => array(
          'label' => 'above',
          'type' => 'micro_og_micro_link_default',
        ),
        'teaser' => array(
          'label' => 'above',
          'type' => 'micro_og_micro_link_default',
        ),
      ),
    ),
  );

  return $items;
}

/**
 * Get all the content types a user should be able to add.
 */
function micro_og_micro_link_field_allowed_values() {
  $options = array();

  foreach (micro_type_get_types() as $micro) {
    if (og_is_group_content_type('micro', $micro->type)) {
      $options[$micro->type] = check_plain($micro->name);
    }
  }

  return $options;
}

/**
 * Implements hook_field_formatter_view().
 */
function micro_og_micro_link_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  if ($field['field_name'] == OG_MICRO_LINK_FIELD && !empty($entity->{OG_MICRO_LINK_FIELD}[LANGUAGE_NONE][0]['value'])) {

    // Get the allowed links.
    $links = array();
    foreach ($entity->{OG_MICRO_LINK_FIELD}[LANGUAGE_NONE] as $delta => $value) {
      $micro_type = $value['value'];
      // Check if the user can create a this micro type.
      if (micro_access('create', $micro_type)) {
        $element[$delta] = array(
          '#type' => 'link',
          '#title' => check_plain(micro_type_get_name($micro_type)),
          '#href' => 'micro/add/' . $micro_type,
          // We know the entity is a micro.
          '#options' => array('query' => array('gids_micro[]' => $entity->mid)),
        );
      }
    }
  }

  return $element;
}

