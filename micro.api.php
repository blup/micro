<?php
// $Id$

/**
 * @file
 * Hooks provided by the Micro module.
 */

/**
 * @defgroup micro_api_hooks Micro API Hooks
 * @{
 * The Micro API allows modules to define micro types, to modify micro
 * types created in the user interface, and to modify micro types created by
 * other modules. It also allows you to define custom behaviors and display modes.
 *
 * Similar to the node.api.php, here is a list of the micro and entity hooks that
 * are invoked, field operations, and other steps that take place during micro
 * operations:
 * - Creating a new micro (calling micro_save() on a new micro):
 *   - field_attach_presave()
 *   - hook_micro_presave() (all)
 *   - Micro records are written to the database
 *   - hook_insert() (micro-type-specific)
 *   - field_attach_insert()
 *   - hook_micro_insert() (all)
 *   - hook_entity_insert() (all)
 * - Updating an existing micro (calling micro_save() on an existing micro):
 *   - field_attach_presave()
 *   - hook_micro_presave() (all)
 *   - Micro records are written to the database
 *   - hook_update() (micro-type-specific)
 *   - field_attach_update()
 *   - hook_micro_update() (all)
 *   - hook_entity_update() (all)
 * - Loading a micro (calling micro_load(), micro_load_multiple(), or
 *   entity_load() with $entity_type of 'micro'):
 *   - Micro is read from database.
 *   - hook_load() (micro-type-specific)
 *   - field_attach_load()
 *   - hook_entity_load() (all)
 *   - hook_micro_load() (all)
 * - Viewing a single micro (calling micro_view() - note that the input to
 *   micro_view() is a loaded micro, so the Loading steps above are already
 *   done):
 *   - hook_view() (micro-type-specific)
 *   - field_attach_prepare_view()
 *   - hook_entity_prepare_view() (all)
 *   - field_attach_view()
 *   - hook_micro_view() (all)
 * - Viewing multiple micros (calling micro_view_multiple() - note that the input
 *   to micro_view_multiple() is a set of loaded micros, so the Loading steps
 *   above are already done):
 *   - field_attach_prepare_view()
 *   - hook_entity_prepare_view() (all)
 *   - hook_view() (micro-type-specific)
 *   - field_attach_view()
 *   - hook_micro_view() (all)
 *   - hook_micro_view_alter() (all)
 * - Deleting a micro (calling micro_delete() or micro_delete_multiple()):
 *   - Micro is loaded (see Loading section above)
 *   - Micro is deleted from database
 *   - hook_delete() (micro-type-specific)
 *   - hook_micro_delete() (all)
 *   - field_attach_delete()
 * - Preparing a micro for editing (calling micro_form() - note that if it's
 *   an existing micro, it will already be loaded; see the Loading section
 *   above):
 *   - hook_prepare() (micro-type-specific)
 *   - hook_micro_prepare() (all)
 *   - hook_form() (micro-type-specific)
 *   - field_attach_form()
 * - Validating a micro during editing form submit (calling
 *   micro_form_validate()):
 *   - hook_validate() (micro-type-specific)
 *   - hook_micro_validate() (all)
 *   - field_attach_form_validate()
 * - Searching (calling micro_search_execute()):
 *   - hook_ranking() (all)
 *   - Query is executed to find matching micros
 *   - Resulting micro is loaded (see Loading section above)
 *   - Resulting micro is prepared for viewing (see Viewing a single micro above)
 *   - comment_micro_update_index() is called.
 *   - hook_micro_search_result() (all)
 * - Search indexing (calling micro_update_index()):
 *   - Micro is loaded (see Loading section above)
 *   - Micro is prepared for viewing (see Viewing a single micro above)
 *   - hook_micro_update_index() (all)
 * @}
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Add mass micro operations.
 *
 * This hook enables modules to inject custom operations into the mass
 * operations dropdown found at admin/micro, by associating a callback
 * function with the operation, which is called when the form is submitted. The
 * callback function receives one initial argument, which is an array of the
 * checked micros.
 *
 * @return
 *   An array of operations. Each operation is an associative array that may
 *   contain the following key-value pairs:
 *   - 'label': Required. The label for the operation, displayed in the dropdown
 *     menu.
 *   - 'callback': Required. The function to call for the operation.
 *   - 'callback arguments': Optional. An array of additional arguments to pass
 *     to the callback function.
 */
function hook_micro_operations() {
  $operations = array(
    'publish' => array(
      'label' => t('Publish selected content'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('status' => MICRO_PUBLISHED)),
    ),
    'unpublish' => array(
      'label' => t('Unpublish selected content'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('status' => MICRO_NOT_PUBLISHED)),
    ),
    'promote' => array(
      'label' => t('Promote selected content to front page'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('status' => MICRO_PUBLISHED, 'promote' => MICRO_PROMOTED)),
    ),
    'demote' => array(
      'label' => t('Demote selected content from front page'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('promote' => MICRO_NOT_PROMOTED)),
    ),
    'sticky' => array(
      'label' => t('Make selected content sticky'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('status' => MICRO_PUBLISHED, 'sticky' => MICRO_STICKY)),
    ),
    'unsticky' => array(
      'label' => t('Make selected content not sticky'),
      'callback' => 'micro_mass_update',
      'callback arguments' => array('updates' => array('sticky' => MICRO_NOT_STICKY)),
    ),
    'delete' => array(
      'label' => t('Delete selected content'),
      'callback' => NULL,
    ),
  );
  return $operations;
}

/**
 * Respond to micro deletion.
 *
 * This hook is invoked from micro_delete_multiple() after the micro has been
 * removed from the micro table in the database, after the type-specific
 * hook_delete() has been invoked, and before field_attach_delete() is called.
 *
 * @param $micro
 *   The micro that is being deleted.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_delete($micro) {
  db_delete('mytable')
    ->condition('mid', $micro->mid)
    ->execute();
}

/**
 * Respond to creation of a new micro.
 *
 * This hook is invoked from micro_save() after the micro is inserted into the
 * micro table in the database, after the type-specific hook_insert() is invoked,
 * and after field_attach_insert() is called.
 *
 * @param $micro
 *   The micro that is being created.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_insert($micro) {
  db_insert('mytable')
    ->fields(array(
      'mid' => $micro->mid,
      'extra' => $micro->extra,
    ))
    ->execute();
}

/**
 * Act on micros being loaded from the database.
 *
 * This hook is invoked during micro loading, which is handled by entity_load(),
 * via classes MicroController and DrupalDefaultEntityController. After the micro
 * information is read from the database or the entity cache, hook_load() is
 * invoked on the micro's micro type module, then field_attach_micro_revision()
 * or field_attach_load() is called, then hook_entity_load() is invoked on all
 * implementing modules, and finally hook_micro_load() is invoked on all
 * implementing modules.
 *
 * This hook should only be used to add information that is not in the micro or
 * micro revisions table, not to replace information that is in these tables
 * (which could interfere with the entity cache). For performance reasons,
 * information for all available micros should be loaded in a single query where
 * possible.
 *
 * The $types parameter allows for your module to have an early return (for
 * efficiency) if your module only supports certain micro types. However, if your
 * module defines a micro type, you can use hook_load() to respond to loading
 * of just that micro type.
 *
 * @param $micros
 *   An array of the micros being loaded, keyed by mid.
 * @param $types
 *   An array containing the types of the micros.
 *
 * For a detailed usage example, see microapi_example.module.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_load($micros, $types) {
  $result = db_query('SELECT mid, foo FROM {mytable} WHERE mid IN(:mids)', array(':mids' => array_keys($micros)));
  foreach ($result as $record) {
    $micros[$record->mid]->foo = $record->foo;
  }
}

/**
 * Control access to a micro.
 *
 * Modules may implement this hook if they want to have a say in whether or not
 * a given user has access to perform a given operation on a micro.
 *
 * The administrative account (user ID #1) always passes any access check,
 * so this hook is not called in that case. Users with the "bypass micro access"
 * permission may always view and edit content through the administrative
 * interface.
 *
 * Note that not all modules will want to influence access on all
 * micro types. If your module does not want to actively grant or
 * block access, return MICRO_ACCESS_IGNORE or simply return nothing.
 * Blindly returning FALSE will break other micro access modules.
 *
 * @link http://api.drupal.org/api/group/micro_access/7 More on the micro access system @endlink
 * @ingroup micro_access
 * @param $micro
 *   The micro on which the operation is to be performed, or, if it does
 *   not yet exist, the type of micro to be created.
 * @param $op
 *   The operation to be performed. Possible values:
 *   - "create"
 *   - "delete"
 *   - "update"
 *   - "view"
 * @param $account
 *   A user object representing the user for whom the operation is to be
 *   performed.
 *
 * @return
 *   MICRO_ACCESS_ALLOW if the operation is to be allowed;
 *   MICRO_ACCESS_DENY if the operation is to be denied;
 *   MICRO_ACCESSS_IGNORE to not affect this operation at all.
 */
function hook_micro_access($micro, $op, $account) {
  $type = is_string($micro) ? $micro : $micro->type;

  if (in_array($type, micro_permissions_get_configured_types())) {
    if ($op == 'create' && user_access('create ' . $type . ' content', $account)) {
      return MICRO_ACCESS_ALLOW;
    }

    if ($op == 'update') {
      if (user_access('edit any ' . $type . ' content', $account) || (user_access('edit own ' . $type . ' content', $account) && ($account->uid == $micro->uid))) {
        return MICRO_ACCESS_ALLOW;
      }
    }

    if ($op == 'delete') {
      if (user_access('delete any ' . $type . ' content', $account) || (user_access('delete own ' . $type . ' content', $account) && ($account->uid == $micro->uid))) {
        return MICRO_ACCESS_ALLOW;
      }
    }
  }

  // Returning nothing from this function would have the same effect.
  return MICRO_ACCESS_IGNORE;
}


/**
 * Act on a micro object about to be shown on the add/edit form.
 *
 * This hook is invoked from micro_object_prepare() after the type-specific
 * hook_prepare() is invoked.
 *
 * @param $micro
 *   The micro that is about to be shown on the add/edit form.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_prepare($micro) {
  if (!isset($micro->comment)) {
    $micro->comment = variable_get("comment_$micro->type", COMMENT_MICRO_OPEN);
  }
}

/**
 * Act on a micro being displayed as a search result.
 *
 * This hook is invoked from micro_search_execute(), after micro_load()
 * and micro_view() have been called.
 *
 * @param $micro
 *   The micro being displayed in a search result.
 *
 * @return
 *   Extra information to be displayed with search result.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_search_result($micro) {
  $comments = db_query('SELECT comment_count FROM {micro_comment_statistics} WHERE mid = :mid', array('mid' => $micro->mid))->fetchField();
  return format_plural($comments, '1 comment', '@count comments');
}

/**
 * Act on a micro being inserted or updated.
 *
 * This hook is invoked from micro_save() before the micro is saved to the
 * database.
 *
 * @param $micro
 *   The micro that is being inserted or updated.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_presave($micro) {
  if ($micro->mid && $micro->moderate) {
    // Reset votes when micro is updated:
    $micro->score = 0;
    $micro->users = '';
    $micro->votes = 0;
  }
}

/**
 * Respond to updates to a micro.
 *
 * This hook is invoked from micro_save() after the micro is updated in the micro
 * table in the database, after the type-specific hook_update() is invoked, and
 * after field_attach_update() is called.
 *
 * @param $micro
 *   The micro that is being updated.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_update($micro) {
  db_update('mytable')
    ->fields(array('extra' => $micro->extra))
    ->condition('mid', $micro->mid)
    ->execute();
}

/**
 * Act on a micro being indexed for searching.
 *
 * This hook is invoked during search indexing, after micro_load(), and after
 * the result of micro_view() is added as $micro->rendered to the micro object.
 *
 * @param $micro
 *   The micro being indexed.
 *
 * @return
 *   Array of additional information to be indexed.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_update_index($micro) {
  $text = '';
  $comments = db_query('SELECT subject, comment, format FROM {comment} WHERE mid = :mid AND status = :status', array(':mid' => $micro->mid, ':status' => COMMENT_PUBLISHED));
  foreach ($comments as $comment) {
    $text .= '<h2>' . check_plain($comment->subject) . '</h2>' . check_markup($comment->comment, $comment->format, '', TRUE);
  }
  return $text;
}

/**
 * Perform micro validation before a micro is created or updated.
 *
 * This hook is invoked from micro_validate(), after a user has has finished
 * editing the micro and is previewing or submitting it. It is invoked at the
 * end of all the standard validation steps, and after the type-specific
 * hook_validate() is invoked.
 *
 * To indicate a validation error, use form_set_error().
 *
 * Note: Changes made to the $micro object within your hook implementation will
 * have no effect.  The preferred method to change a micro's content is to use
 * hook_micro_presave() instead. If it is really necessary to change
 * the micro at the validate stage, you can use form_set_value().
 *
 * @param $micro
 *   The micro being validated.
 * @param $form
 *   The form being used to edit the micro.
 * @param $form_state
 *   The form state array.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_validate($micro, $form, &$form_state) {
  if (isset($micro->end) && isset($micro->start)) {
    if ($micro->start > $micro->end) {
      form_set_error('time', t('An event may not end before it starts.'));
    }
  }
}

/**
 * Act on a micro after validated form values have been copied to it.
 *
 * This hook is invoked when a micro form is submitted with either the "Save" or
 * "Preview" button, after form values have been copied to the form state's micro
 * object, but before the micro is saved or previewed. It is a chance for modules
 * to adjust the micro's properties from what they are simply after a copy from
 * $form_state['values']. This hook is intended for adjusting non-field-related
 * properties. See hook_field_attach_submit() for customizing field-related
 * properties.
 *
 * @param $micro
 *   The micro being updated in response to a form submission.
 * @param $form
 *   The form being used to edit the micro.
 * @param $form_state
 *   The form state array.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_submit($micro, $form, &$form_state) {
  // Decompose the selected menu parent option into 'menu_name' and 'plid', if
  // the form used the default parent selection widget.
  if (!empty($form_state['values']['menu']['parent'])) {
    list($micro->menu['menu_name'], $micro->menu['plid']) = explode(':', $form_state['values']['menu']['parent']);
  }
}

/**
 * Act on a micro that is being assembled before rendering.
 *
 * The module may add elements to $micro->content prior to rendering. This hook
 * will be called after hook_view(). The structure of $micro->content is a
 * renderable array as expected by drupal_render().
 *
 * When $view_mode is 'rss', modules can also add extra RSS elements and
 * namespaces to $micro->rss_elements and $micro->rss_namespaces respectively for
 * the RSS item generated for this micro.
 * For details on how this is used, see micro_feed().
 *
 * @see blog_micro_view()
 * @see forum_micro_view()
 * @see comment_micro_view()
 *
 * @param $micro
 *   The micro that is being assembled for rendering.
 * @param $view_mode
 *   The $view_mode parameter from micro_view().
 * @param $langcode
 *   The language code used for rendering.
 *
 * @see hook_entity_view()
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_view($micro, $view_mode, $langcode) {
  $micro->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * Alter the results of micro_view().
 *
 * This hook is called after the content has been assembled in a structured
 * array and may be used for doing processing which requires that the complete
 * micro content structure has been built.
 *
 * If the module wishes to act on the rendered HTML of the micro rather than the
 * structured content array, it may use this hook to add a #post_render
 * callback.  Alternatively, it could also implement hook_preprocess_micro(). See
 * drupal_render() and theme() documentation respectively for details.
 *
 * @param $build
 *   A renderable array representing the micro content.
 *
 * @see micro_view()
 * @see hook_entity_view_alter()
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_view_alter(&$build) {
  if ($build['#view_mode'] == 'full' && isset($build['an_additional_field'])) {
    // Change its weight.
    $build['an_additional_field']['#weight'] = -10;
  }

  // Add a #post_render callback to act on the rendered HTML of the micro.
  $build['#post_render'][] = 'my_module_micro_post_render';
}

/**
 * Define module-provided micro types.
 *
 * This hook allows a module to define one or more of its own micro types. For
 * example, the blog module uses it to define a blog micro-type named "Blog
 * entry." The name and attributes of each desired micro type are specified in
 * an array returned by the hook.
 *
 * Only module-provided micro types should be defined through this hook. User-
 * provided (or 'custom') micro types should be defined only in the 'micro_type'
 * database table, and should be maintained by using the micro_type_save() and
 * micro_type_delete() functions.
 *
 * @return
 *   An array of information defining the module's micro types. The array
 *   contains a sub-array for each micro type, with the machine-readable type
 *   name as the key. Each sub-array has up to 10 attributes. Possible
 *   attributes:
 *   - "name": the human-readable name of the micro type. Required.
 *   - "base": the base string used to construct callbacks corresponding to
 *      this micro type.
 *      (i.e. if base is defined as example_foo, then example_foo_insert will
 *      be called when inserting a micro of that type). This string is usually
 *      the name of the module, but not always. Required.
 *   - "description": a brief description of the micro type. Required.
 *   - "help": help information shown to the user when creating a micro of
 *      this type.. Optional (defaults to '').
 *   - "has_title": boolean indicating whether or not this micro type has a title
 *      field. Optional (defaults to TRUE).
 *   - "title_label": the label for the title field of this micro type.
 *      Optional (defaults to 'Title').
 *   - "locked": boolean indicating whether the administrator can change the
 *      machine name of this type. FALSE = changeable (not locked),
 *      TRUE = unchangeable (locked). Optional (defaults to TRUE).
 *
 * The machine-readable name of a micro type should contain only letters,
 * numbers, and underscores. Underscores will be converted into hyphens for the
 * purpose of constructing URLs.
 *
 * All attributes of a micro type that are defined through this hook (except for
 * 'locked') can be edited by a site administrator. This includes the
 * machine-readable name of a micro type, if 'locked' is set to FALSE.
 *
 * @ingroup micro_api_hooks
 */
function hook_micro_info() {
  return array(
    'blog' => array(
      'name' => t('Blog entry'),
      'base' => 'blog',
      'description' => t('Use for multi-user blogs. Every user gets a personal blog.'),
    )
  );
}

/**
 * Provide additional methods of scoring for core search results for micros.
 *
 * A micro's search score is used to rank it among other micros matched by the
 * search, with the highest-ranked micros appearing first in the search listing.
 *
 * For example, a module allowing users to vote on content could expose an
 * option to allow search results' rankings to be influenced by the average
 * voting score of a micro.
 *
 * All scoring mechanisms are provided as options to site administrators, and
 * may be tweaked based on individual sites or disabled altogether if they do
 * not make sense. Individual scoring mechanisms, if enabled, are assigned a
 * weight from 1 to 10. The weight represents the factor of magnification of
 * the ranking mechanism, with higher-weighted ranking mechanisms having more
 * influence. In order for the weight system to work, each scoring mechanism
 * must return a value between 0 and 1 for every micro. That value is then
 * multiplied by the administrator-assigned weight for the ranking mechanism,
 * and then the weighted scores from all ranking mechanisms are added, which
 * brings about the same result as a weighted average.
 *
 * @return
 *   An associative array of ranking data. The keys should be strings,
 *   corresponding to the internal name of the ranking mechanism, such as
 *   'recent', or 'comments'. The values should be arrays themselves, with the
 *   following keys available:
 *   - "title": the human readable name of the ranking mechanism. Required.
 *   - "join": part of a query string to join to any additional necessary
 *     table. This is not necessary if the table required is already joined to
 *     by the base query, such as for the {micro} table. Other tables should use
 *     the full table name as an alias to avoid naming collisions. Optional.
 *   - "score": part of a query string to calculate the score for the ranking
 *     mechanism based on values in the database. This does not need to be
 *     wrapped in parentheses, as it will be done automatically; it also does
 *     not need to take the weighted system into account, as it will be done
 *     automatically. It does, however, need to calculate a decimal between
 *     0 and 1; be careful not to cast the entire score to an integer by
 *     inadvertently introducing a variable argument. Required.
 *   - "arguments": if any arguments are required for the score, they can be
 *     specified in an array here.
 *
 * @ingroup micro_api_hooks
 */
function hook_ranking() {
  // If voting is disabled, we can avoid returning the array, no hard feelings.
  if (variable_get('vote_micro_enabled', TRUE)) {
    return array(
      'vote_average' => array(
        'title' => t('Average vote'),
        // Note that we use i.sid, the search index's search item id, rather than
        // n.mid.
        'join' => 'LEFT JOIN {vote_micro_data} vote_micro_data ON vote_micro_data.mid = i.sid',
        // The highest possible score should be 1, and the lowest possible score,
        // always 0, should be 0.
        'score' => 'vote_micro_data.average / CAST(%f AS DECIMAL)',
        // Pass in the highest possible voting score as a decimal argument.
        'arguments' => array(variable_get('vote_score_max', 5)),
      ),
    );
  }
}


/**
 * Respond to micro type creation.
 *
 * This hook is invoked from micro_type_save() after the micro type is added
 * to the database.
 *
 * @param $info
 *   The micro type object that is being created.
 */
function hook_micro_type_insert($info) {
}

/**
 * Respond to micro type updates.
 *
 * This hook is invoked from micro_type_save() after the micro type is updated
 * in the database.
 *
 * @param $info
 *   The micro type object that is being updated.
 */
function hook_micro_type_update($info) {
  if (!empty($info->old_type) && $info->old_type != $info->type) {
    $setting = variable_get('comment_' . $info->old_type, COMMENT_MICRO_OPEN);
    variable_del('comment_' . $info->old_type);
    variable_set('comment_' . $info->type, $setting);
  }
}

/**
 * Respond to micro type deletion.
 *
 * This hook is invoked from micro_type_delete() after the micro type is removed
 * from the database.
 *
 * @param $info
 *   The micro type object that is being deleted.
 */
function hook_micro_type_delete($info) {
  variable_del('comment_' . $info->type);
}

/**
 * @} End of "addtogroup hooks".
 */
