<?php
// $Id$

/**
 * @file
 * Contains the micro view row style plugin.
 */

/**
 * Plugin which performs a micro_view on the resulting object.
 *
 * Most of the code on this object is in the theme function.
 */
class micro_plugin_row_micro_view extends views_plugin_row {
  // Basic properties that let the row style follow relationships.
  var $base_table = 'micro';
  var $base_field = 'mid';

  // Stores the micros loaded with pre_render.
  var $micros = array();

  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);
    // Handle existing views with the deprecated 'teaser' option.
    if (isset($this->options['small'])) {
      $this->options['build_mode'] = $this->options['small'] ? 'small' : 'full';
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['build_mode'] = array('default' => 'small');
    $options['links'] = array('default' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // CCK holds the registry of available build modes, but can hardly
    // push them as options for the micro row style, so we break the normal
    // rule of not directly relying on non-core modules.
    if ($modes = module_invoke('content', 'build_modes')) {
      $options = array();
      foreach ($modes as $key => $value) {
        if (isset($value['views style']) && $value['views style']) {
          $options[$key] = $value['title'];
        }
      }
    }
    else {
      $info = entity_get_info("micro");
      foreach ($info['view modes'] as $key=>$value) {
        if ($key != 'search_index' && $key != 'search_result') {
          $options[$key] = $value['label'];
        }
      }
    }
    $form['build_mode'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('Build mode'),
      '#default_value' => $this->options['build_mode'],
     );
    $form['links'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display links'),
      '#default_value' => $this->options['links'],
    );
  }

  function pre_render($values) {
    $mids = array();
    foreach ($values as $row) {
      $mids[] = $row->{$this->field_alias};
    }
    $this->micros = micro_load_multiple($mids);
  }
}
