<?php
// $Id$ micro_views_handler_field_edit.inc, v 1.0 2010/12/01 04:20:00 blup Exp $

/**
 * @file
 * Contains an edit link for micro items.
 */

/**
 * Field handler to add a field with an edit link.
 */
class micro_views_handler_field_edit extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['mtid'] = 'mtid';
    $this->additional_fields['eid'] = 'eid';
    $this->additional_fields['uid'] = 'uid';
  }
  function render($values) {
    $micro = new Micro();
    $micro->mtid = $values->{$this->aliases['mtid']};
    $micro->eid = $values->{$this->aliases['eid']};
    $micro->uid = $values->{$this->aliases['uid']};
    $micro->mid = $values->{$this->field_alias};
    /*
     *if (!micro_access('edit', $micro)) {
     *  return;
     *}
     */
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    return l($text, "micro/$micro->mid/edit", array('query' => drupal_get_destination()));
  }
}
