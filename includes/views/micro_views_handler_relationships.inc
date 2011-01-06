<?php
// $Id$

/**
 * @file
 * Contains various relationship handlers.
 */

/**
 * Relationship from entity to micro.
 *
 * @ingroup views
 */
class micro_views_handler_relationship_micro extends views_handler_relationship {

  function option_definition() {
    $options = parent::option_definition();

    $entity_type = $this->definition['entity_type'];
    /*
     *$options['entity'] = array('default' => micro_types_from_entity($entity_type));
     */
    $options['micro'] = array('default' => NULL);
    $options['required'] = array('default' => 1);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $entity_type = $this->definition['entity_type'];
    $form['label']['#default_value'] = 'micro';
    $form['label']['#description'] .= ' ' . t('The name of the selected micro type makes a good label.');
    $form['micro'] = micro_views_config_form('radios', $entity_type, $this->options['micro']);

    $form['required']['#title'] = t('Include only content with micro items of this type');
    $form['required']['#description'] = t('If checked, only content with micro items of this type will be included. Leave unchecked to include all content.');

    if (!$form['micro']['#options']) {
      $form = array(
        'error' => array(
          '#markup' => '<p class="error form-item">' . t('No %type micro type exist. You must first <a href="@create-url">create a %type micro type</a> before being able to use this relationship type.', array('%type' => $entity_type, '@create-url' => url(MICRO_ADMIN_PATH))) . '</p>',
        ),
      );
      $form_state['no micro type exist'] = TRUE;
    }
  }

  function options_validate($form, &$form_state) {
    if (!empty($form_state['no micro type exist'])) {
      form_error($form, t('You must first create a micro type'));
    }
  }

  function admin_summary() {
    return $this->options['micro'];
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    $micro_type = micro_type_get_type($this->options['micro']);
    $this->definition['extra'][] = array(
      'field' => 'mtid',
      'value' => $micro_type->mtid,
      'numeric' => TRUE,
    );
    parent::query();
  }
}

/**
 * Relationship from micro items to their entities.
 *
 * @ingroup views
 */
class micro_views_handler_relationship_entity extends views_handler_relationship {

  function option_definition() {
    $options = parent::option_definition();

    $entity_type = $this->definition['entity_type'];
    /*
     *$options['entity'] = array('default' => micro_types_from_entity($entity_type));
     */
    $options['user_scope'] = array('default' => 'current');
    $options['required'] = array('default' => 1);
    $options['micro'] = array('default' => NULL);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $entity_type = $this->definition['entity_type'];
    $form['label']['#default_value'] = $entity_type;
    $form['label']['#description'] .= ' ' . t('The name of the selected micro type makes a good label.');
    $form['micro'] = micro_views_config_form('radios', $entity_type, $this->options['micro']);

    $form['user_scope'] = array(
      '#type' => 'radios',
      '#title' => t('By'),
      '#options' => array('current' => t('Current user'), 'any' => t('Any user')),
      '#default_value' => $this->options['user_scope'],
    );

    $form['required']['#title'] = t('Include only micro items of this type');
    $form['required']['#description'] = t('If checked, only micro items of this type will be included. Leave unchecked to include all micro items; or, in combination with the <em>micro type</em> filter, to limit the results to a specific micro type.');

    if (!$form['micro']['#options']) {
      $form = array(
        'error' => array(
          '#markup' => '<p class="error form-item">' . t('No %type micro type exist. You must first <a href="@create-url">create a %type micro type</a> before being able to use this relationship type.', array('%type' => $entity_type, '@create-url' => url(MICRO_ADMIN_PATH))) . '</p>',
        ),
      );
      $form_state['no micro type exist'] = TRUE;
    }
  }

  function options_validate($form, &$form_state) {
    if (!empty($form_state['no micro type exist'])) {
      form_error($form, t('You must first create a micro type'));
    }
  }

  function admin_summary() {
    return $this->options['micro'];
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    $micro_type = micro_type_get_type($this->options['micro']);
    $table_data = views_fetch_data($this->definition['base']);
    $base_field = empty($this->definition['base field']) ? $table_data['table']['base']['field'] : $this->definition['base field'];
    $this->ensure_my_table();

    $def = $this->definition;

    $def['table'] = $this->definition['base'];
    $def['field'] = $base_field;
    $def['left_table'] = $this->table_alias;
    $def['left_field'] = $this->field;
    $def['type'] = empty($this->options['required']) ? 'LEFT' : 'INNER';

    $join = new views_join();

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    // use a short alias for this:
    $alias = $def['table'] . '_' . $this->table;

    $this->query->add_where($this->options['group'], "$this->table_alias.mtid", $micro_type->mtid);
    parent::query();
  }
}
