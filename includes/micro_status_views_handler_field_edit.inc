<?php
// $Id$

/**
 * @file
 *   Provides an edit link to Views.
 */

/**
 * Field handler to add a field with an edit link.
 */
class micro_views_handler_field_edit extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
    $this->additional_fields['eid'] = 'eid';
    $this->additional_fields['uid'] = 'uid';
  }
  function render($values) {
    $micro = new Micro();
    $micro->type = $values->{$this->aliases['type']};
    $micro->eid = $values->{$this->aliases['eid']};
    $micro->uid = $values->{$this->aliases['uid']};
    $micro->mid = $values->{$this->field_alias};
    if (!micro_access('edit', $micro)) {
      return;
    }
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    return l($text, "micro/$micro->mid/edit", array('query' => drupal_get_destination()));
  }
}
