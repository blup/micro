<?php
// $Id$

/**
 * @file
 * Contains a micro path field.
 */

/**
 * Field handler to provide simple renderer that allows linking to a micro.
 */
class micro_views_handler_field_micro extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (!empty($this->options['link_to_micro'])) {
      $this->additional_fields['mid'] = array('table' => 'micro', 'field' => 'mid');
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_micro'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Provide link to micro option
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_micro'] = array(
      '#title' => t('Link this field to its micro'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_micro']),
    );
  }

  /**
   * Render whatever the data is as a link to the micro.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_micro']) && $data !== NULL && $data !== '') {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "micro/" . $values->{$this->aliases['mid']};
    }
    else {
      $this->options['alter']['make_link'] = FALSE;
    }
    return $data;
  }

  function render($values) {
    return $this->render_link(check_plain($values->{$this->field_alias}), $values);
  }
}
