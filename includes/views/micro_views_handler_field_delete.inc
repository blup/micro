<?php
// $Id$

/**
 * @file
 * Contains a delete link for micro items.
 */

/**
 * Field handler to add a field with an edit link.
 */
class micro_views_handler_field_delete extends views_handler_field {
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
     *if (!micro_access('delete', $micro)) {
     *  return;
     *}
     */
    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return l($text, "micro/$micro->mid/delete", array('query' => drupal_get_destination()));
  }
}
