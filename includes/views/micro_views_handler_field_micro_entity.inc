<?php
// $Id$

/**
 * @file
 * Contains a micro entity field.
 */

/**
 * Field handler to translate a micro entity into its readable form.
 */
class micro_views_handler_field_micro_entity extends views_handler_field {
  function render($values) {
    return check_plain($values->{$this->field_alias});
  }
}

