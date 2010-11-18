<?php
// $Id$

/**
 * @file
 * Provide views data and handlers for the Facebook-style Statuses module.
 */

/**
 * Implementation of hook_views_data().
 */
function micro_views_data() {

  // ----------------------------------------------------------------------
  // Micro table

  //Basic table information.
  $data['micro']['table']['group']  = t('Micro');
  $data['micro']['table']['join'] = array(
    'micro_type' => array(
      'left_field' => 'type',
      'field' => 'type',
      'type' => 'INNER',
    ),
  );

  //Advertise this table as a possible base table.
  $data['micro']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Micro'),
    'help' => t('Micro items are attacheable to other fieldable entities.'),
    'weight' => 10,
  );

  /*
   *$data['micro']['users'] = array(
   *  'group' => t('Facebook-style Statuses'),
   *  'relationship' => array(
   *    'title' => t('Users'),
   *    'label' => t('Users'),
   *    'help' => t('Add a relationship to gain access to information related to the users who submitted the relevant statuses.'),
   *    'relationship table' => 'users',
   *    'relationship field' => 'uid',
   *    'base' => 'micro',
   *    'field' => 'uid',
   *    'type' => 'INNER',
   *    'handler' => 'views_handler_relationship',
   *  ),
   *);
   */

  //Declares the micro id column.
  $data['micro']['mid'] = array(
    'title' => t('Micro ID'),
    'help' => t('The ID of the micro item.'),
    'field' => array(
    //TODO
      'handler' => 'micro_views_handler_field_micro',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  //Alias for micro id to extend its use.
  /*
   *$data['micro']['mid_extra'] = array(
   *  'title' => t('Last Micro per User'),
   *  'help' => t('Show only the last status update for each user.'),
   *  'filter' => array(
   *    'field' => 'mid',
   *    'handler' => 'micro_views_handler_filter',
   *    'label' => t('Last Micro per User'),
   *  ),
   *);
   */

  // Micro type
  //TODO
  $data['micro']['type'] = array(
    'title' => t('Type'),
    'help' => t('The type of a micro item.'),
    'field' => array(
      'handler' => 'micro_views_handler_field_micro_type',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'micro_views_handler_filter_micro_type',
    ),
    'argument' => array(
      'handler' => 'micro_views_handler_argument_micro_type',
    ),
  );

  $data['micro']['edit'] = array(
    'title' => t('Edit'),
    'help' => t('Shows a link to edit the micro item to users with permission to see it.'),
    'field' => array(
      'field' => 'mid',
      'handler' => 'micro_views_handler_field_edit',
      'click sortable' => FALSE,
    ),
  );
  $data['micro']['delete'] = array(
    'title' => t('Delete'),
    'help' => t('Shows a link to delete the micro item to users with permission to see it.'),
    'field' => array(
      'field' => 'mid',
      'handler' => 'micro_views_handler_field_delete',
      'click sortable' => FALSE,
    ),
  );

//TODO
  $data['micro']['path'] = array(
    'field' => array(
      'title' => t('Path'),
      'help' => t('The aliased path to this micro.'),
      'handler' => 'micro_views_handler_field_micro_path',
    ),
  );

  /*
   *$data['micro']['repost'] = array(
   *  'title' => t('Re-post'),
   *  'help' => t('Shows a link to re-post a status.'),
   *  'field' => array(
   *    'field' => 'mid',
   *    'handler' => 'micro_views_handler_field_repost',
   *    'click sortable' => FALSE,
   *  ),
   *);
   */

  //Declares the author UID column.
  $data['micro']['uid'] = array(
    'title' => t('Author ID'),
    'help' => t('The user ID of the author of the micro item.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  //Alias for author UID to use the name.
  $data['micro']['uid_name'] = array(
    'title' => t('Author name'),
    'help' => t('The name of the user who created the micro item.'),
    'field' => array(
      'field' => 'pid',
      'handler' => 'micro_views_handler_field_author',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'field' => 'pid',
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'field' => 'pid',
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'field' => 'pid',
      'handler' => 'views_handler_argument_user_uid',
    ),
  );
  //Alias for author picture.
  $data['micro']['uid_pic'] = array(
    'title' => t('Author picture'),
    'help' => t('The picture of the user who created the micro item.'),
    'field' => array(
      'field' => 'pid',
      'handler' => 'micro_views_handler_field_author_pic',
      'click sortable' => FALSE,
    ),
  );

  //Alias for extras for the author.
  /*
   *$data['micro']['pid_extra'] = array(
   *  'title' => t('Usernames (context-aware)'),
   *  'help' => t('The themed username(s) of either the owner or the sender and recipient of the status, depending on the context.'),
   *  'field' => array(
   *    'field' => 'pid',
   *    'handler' => 'micro_views_handler_field_cross',
   *    'click sortable' => FALSE,
   *    'label' => t('Users'),
   *  ),
   *);
   *$data['micro']['pid_extra_2'] = array(
   *  'title' => t('Not own statuses'),
   *  'help' => t("Do not show statuses posted to one's own profile."),
   *  'filter' => array(
   *    'field' => 'pid',
   *    'handler' => 'micro_views_handler_filter_not_own',
   *    'label' => t("Restrict to others' statuses"),
   *  ),
   *);
   */
  //This is slightly different than, but could probably be combined with, pid_extra_2.
  /*
   *$data['micro']['pid_extra_3'] = array(
   *  'title' => t('Only own statuses'),
   *  'help' => t("Only show statuses posted to one's own profile."),
   *  'filter' => array(
   *    'field' => 'pid',
   *    'handler' => 'micro_views_handler_filter_own',
   *    'label' => t('Restrict to own statuses'),
   *  ),
   *);
   *$data['micro']['crosspost_users_pics'] = array(
   *  'title' => t('Users with Pictures'),
   *  'help' => t('The themed username(s) and pictures of either the owner or the sender and recipient of the status, depending on the context.'),
   *  'field' => array(
   *    'field' => 'pid',
   *    'handler' => 'micro_views_handler_field_cross_pic',
   *    'click sortable' => FALSE,
   *    'label' => t('Users with Pictures'),
   *  ),
   *);
   *$data['micro']['respond'] = array(
   *  'title' => t('Respond link'),
   *  'help' => t('Links to view the conversation or respond to it when appropriate.'),
   *  'field' => array(
   *    'field' => 'pid',
   *    'handler' => 'micro_views_handler_field_respond',
   *    'click sortable' => FALSE,
   *    'label' => t('Respond to conversation'),
   *  ),
   *);
   */

  //Declares the micro item timestamp column.
  $data['micro']['created'] = array(
    'title' => t('Created time'),
    'help' => t('The time the micro item was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['micro']['created_fulldate'] = array(
    'title' => t('Created date'),
    'help' => t('In the form of CCYYMMDD.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_fulldate',
    ),
  );

  $data['micro']['created_year_month'] = array(
    'title' => t('Created year + month'),
    'help' => t('In the form of YYYYMM.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year_month',
    ),
  );

  $data['micro']['created_year'] = array(
    'title' => t('Created year'),
    'help' => t('In the form of YYYY.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year',
    ),
  );

  $data['micro']['created_month'] = array(
    'title' => t('Created month'),
    'help' => t('In the form of MM (01 - 12).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_month',
    ),
  );

  $data['micro']['created_day'] = array(
    'title' => t('Created day'),
    'help' => t('In the form of DD (01 - 31).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_day',
    ),
  );

  $data['micro']['created_week'] = array(
    'title' => t('Created week'),
    'help' => t('In the form of WW (01 - 53).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_week',
    ),
  );

  /*
   *if (module_exists('flag')) {
   *  $data['micro']['user-flag-plus-current'] = array(
   *    'title' => t('Content from flagged users or the current user'),
   *    'filter' => array(
   *      'field' => 'pid',
   *      'handler' => 'micro_views_handler_filter_flagged_user',
   *    ),
   *  );
   *  $data['micro']['user-flag-plus-arg'] = array(
   *    'title' => t('Content from flagged users or the argument user'),
   *    'argument' => array(
   *      'field' => 'pid',
   *      'handler' => 'micro_views_handler_argument_flagged_user',
   *    ),
   *  );
   *}
   */

  // ----------------------------------------------------------------------
  // Micro type table

  $data['micro_type']['table']['group']  = t('Micro type');
  $data['micro_type']['table']['join'] = array(
    'micro' => array(
      'left_field' => 'type',
      'field' => 'type',
      'type' => 'INNER',
    ),
  );
  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function micro_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'micro'),
    ),
    'handlers' => array(
      /*
       *'micro_views_handler_argument_flagged_user' => array(
       *  'parent' => 'views_handler_argument',
       *),
       *'micro_views_handler_field_created' => array(
       *  'parent' => 'views_handler_field_date',
       *),
       */
      /*
       *'micro_views_handler_field_cross' => array(
       *  'parent' => 'views_handler_field',
       *),
       *'micro_views_handler_field_cross_pic' => array(
       *  'parent' => 'views_handler_field',
       *),
       */
      'micro_views_handler_field_delete' => array(
        'parent' => 'views_handler_field',
      ),
      'micro_views_handler_field_edit' => array(
        'parent' => 'views_handler_field',
      ),
      'micro_views_handler_field_author' => array(
        'parent' => 'views_handler_field_user',
      ),
      'micro_views_handler_field_author_pic' => array(
        'parent' => 'views_handler_field',
      ),
      /*
       *'micro_views_handler_field_author_pic_activity' => array(
       *  'parent' => 'views_handler_field',
       *),
       *'micro_views_handler_field_repost' => array(
       *  'parent' => 'views_handler_field',
       *),
       *'micro_views_handler_field_respond' => array(
       *  'parent' => 'views_handler_field',
       *),
       *'micro_views_handler_field_status' => array(
       *  'parent' => 'views_handler_field',
       *),
       *'micro_views_handler_filter' => array(
       *  'parent' => 'views_handler_filter_boolean_operator',
       *),
       *'micro_views_handler_filter_flagged_user' => array(
       *  'parent' => 'views_handler_filter',
       *),
       *'micro_views_handler_filter_not_own' => array(
       *  'parent' => 'views_handler_filter_boolean_operator',
       *),
       *'micro_views_handler_filter_own' => array(
       *  'parent' => 'views_handler_filter_boolean_operator',
       *),
       */
    ),
  );
}

