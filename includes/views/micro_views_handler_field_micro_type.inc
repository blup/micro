<?php
// $Id$ micro_views_handler_field_micro_type.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Contains a micro type field.
 */

/**
 * Field handler to translate a micro type into its readable form.
 */
class micro_views_handler_field_micro_type extends micro_views_handler_field_micro {
  function option_definition() {
    $options = parent::option_definition();
    $options['machine_name'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Provide machine_name option for to micro type display.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['machine_name'] = array(
      '#title' => t('Output machine name'),
      '#description' => t('Display field as the micro type machine name.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['machine_name']),
    );
  }

  /**
    * Render micro type as human readable name, unless using machine_name option.
    */
  function render_name($data, $values) {
    if ($this->options['machine_name'] != 1 && $data !== NULL && $data !== '') {
      return t(check_plain(micro_type_get_name($data)));
    }
    return check_plain($data);
  }

  function render($values) {
    return $this->render_link($this->render_name($values->{$this->field_alias}, $values), $values);
  }
}

