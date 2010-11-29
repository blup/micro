<?php
// $Id$ micro_views_handler_field_author.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Contains an author's name field.
 */

/**
 * Allows linking to the author of the status.
 */
class micro_views_handler_field_author extends views_handler_field_user {
  function render($values) {
    $account = user_load($values->{$this->field_alias});
    if (!empty($this->options['link_to_user'])) {
      return theme('username', $account);
    }
    return check_plain($account->name);
  }
}
