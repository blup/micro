<?php
// $Id$ micro_views_handler_argument_micro_type.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Contains a micro type argument.
 */

/**
 * Argument handler to accept a micro type.
 */
class micro_views_handler_argument_micro_type extends views_handler_argument {
  function construct() {
    parent::construct('type');
  }

  /**
   * Override the behavior of summary_name(). Get the user friendly version
   * of the micro type.
   */
  function summary_name($data) {
    return $this->micro_type($data->{$this->name_alias});
  }

  /**
   * Override the behavior of title(). Get the user friendly version of the
   * micro type.
   */
  function title() {
    return $this->micro_type($this->argument);
  }

  function micro_type($type) {
    $output = micro_type_get_name($type);
    if (empty($output)) {
      $output = t('Unknown micro type');
    }
    return check_plain($output);
  }
}
