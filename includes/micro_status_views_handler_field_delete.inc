<?php
// $Id$

/**
 * @file
 *   Provides an edit link to Views.
 */

/**
 * Field handler to add a field with an edit link.
 */
class micro_views_handler_field_delete extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
    $this->additional_fields['uid'] = 'uid';
    $this->additional_fields['pid'] = 'pid';
  }
  function render($values) {
    $micro = new Micro();
    $micro->type = $values->{$this->aliases['type']};
    $micro->eid = $values->{$this->aliases['eid']};
    $micro->uid = $values->{$this->aliases['uid']};
    $micro->mid = $values->{$this->field_alias};
    if (!micro_access('delete', $micro)) {
      return;
    }
    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return l($text, "micro/$micro->mid/delete", array('query' => drupal_get_destination()));
  }
}
