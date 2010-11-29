<?php
// $Id$ micro_views_handler_filter_micro_entity.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Contains a micro type by entity filter.
 */

/**
 * Filter by micro entity
 */
class micro_views_handler_filter_micro_entity extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Micro entity');
      $entities = micro_get_entities(FALSE);
      foreach ($entities as $entity => $label) {
        $options[$entity] = t($label);
      }
      $this->value_options = $options;
    }
  }
}

