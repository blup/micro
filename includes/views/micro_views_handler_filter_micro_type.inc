<?php
// $Id$

/**
 * @file
 * Contains a micro type filter.
 */

/**
 * Filter by micro type
 */
class micro_views_handler_filter_micro_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Micro type');
      $types = micro_type_get_types();
      foreach ($types as $type => $info) {
        $options[$type] = t($info->name);
      }
      $this->value_options = $options;
    }
  }
}
