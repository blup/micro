<?php
// $Id$

/**
 * @file
 * Default theme implementation to display a micro.
 *
 * Available variables:
 * - $title: Title of the micro (micro ID).
 * - $content: An array of containing the micro item's content.
 * - $picture: The micro author's picture from user-picture.tpl.php.
 * - $date: Formatted creation date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $created variable.
 * - $name: Themed username of micro author output from theme_username().
 * - $permalink: Direct url of the current micro.
 * - $display_submitted: whether submission information should be displayed.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - micro: The current template type, i.e., "theming hook".
 *   - micro-[type]: The current micro type. For example, if the micro is a
 *     "Blog entry" it would result in "micro-blog". Note that the machine
 *     name will often be in a short form of the human readable label.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Other variables:
 * - $micro: Full micro object. Contains data that may not be safe.
 * - $micro_type: Full micro type object, i.e. status, story, page, microblog, etc.
 * - $type: Machine name of the micro type.
 * - $entity: Full entity object to which the micro is attached to.
 * - $mid: Micro ID.
 * - $mtid: Micro type ID.
 * - $eid: Entity ID to which the micro is attached to.
 * - $uid: User ID of the micro author.
 * - $created: Time the micro was published formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes. The default values can be one
 *   or more of the following:
 *   - micro: The current template type, i.e., "theming hook".
 *   - micro-by-entity-author: micro by the author of the parent entity.
 *   - micro-by-viewer: micro by the user currently viewing the page.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the micro. Increments each time it's output.
 *
 * Micro status variables:
 * - $view_mode: View mode, e.g. 'full', 'small'...
 * - $page: Flag for the full page state.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * Field variables: for each field instance attached to the micro a corresponding
 * variable is defined, e.g. $micro->body becomes $body. When needing to access
 * a field's raw values, developers/themers are strongly encouraged to use these
 * variables. Otherwise they will have to explicitly specify the desired field
 * language, e.g. $micro->body['en'], thus overriding any language negotiation
 * rule that was previously applied.
 *
 * @see template_preprocess()
 * @see template_preprocess_micro()
 * @see template_process()
 */
?>
<div id="micro-<?php print $mid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <?php print $user_picture; ?>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $permalink; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <?php if ($display_submitted): ?>
    <div class="submitted">
      <?php
        print t('Submitted by !username on !datetime',
          array('!username' => $name, '!datetime' => $date));
      ?>
    </div>
  <?php endif; ?>

  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the links now so that we can render them later.
      hide($content['links']);
       print render($content['micro_publisher']);
       print render($content);
       print render($content['links']);
    ?>
  </div>

  <?php print render($content['links']); ?>
</div>
