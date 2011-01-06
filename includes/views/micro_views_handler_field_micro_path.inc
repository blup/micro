<?php
// $Id$

/**
 * @file
 * Contains a micro path field.
 */

/**
 * Field handler to present the path to the micro.
 */
class micro_views_handler_field_micro_path extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['absolute'] = array('default' => FALSE);

    return $options;
  }

  function construct() {
    parent::construct();
    $this->additional_fields['mid'] = 'mid';
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['absolute'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use absolute link (begins with "http://")'),
      '#default_value' => $this->options['absolute'],
      '#description' => t('If you want to use this as in "output this field as link" in "link path", you have to enabled this option.'),
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $mid = $values->{$this->aliases['mid']};
    return url("micro/$mid", array('absolute' => $this->options['absolute']));
  }
}
