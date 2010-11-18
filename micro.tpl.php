<?php
// $Id: micro.tpl.php,v 1.33 2010/04/08 18:26:42 dries Exp $

/**
 * @file
 * Default theme implementation to display a micro.
 *
 * Available variables:
 * - $title: the (sanitized) title of the micro.
 * - $content: An array of micro items. Use render($content) to print them all,
 *   or print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $user_picture: The micro author's picture from user-picture.tpl.php.
 * - $date: Formatted creation date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $created variable.
 * - $name: Themed username of micro author output from theme_username().
 * - $micro_url: Direct url of the current micro.
 * - $display_submitted: whether submission information should be displayed.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - micro: The current template type, i.e., "theming hook".
 *   - micro-[type]: The current micro type. For example, if the micro is a
 *     "Blog entry" it would result in "micro-blog". Note that the machine
 *     name will often be in a short form of the human readable label.
 *   - micro-teaser: micros in teaser form.
 *   - micro-preview: micros in preview mode.
 *   The following are controlled through the micro publishing options.
 *   - micro-promoted: micros promoted to the front page.
 *   - micro-sticky: micros ordered above other non-sticky micros in teaser
 *     listings.
 *   - micro-unpublished: Unpublished micros visible only to administrators.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Other variables:
 * - $micro: Full micro object. Contains data that may not be safe.
 * - $type: micro type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the micro.
 * - $uid: User ID of the micro author.
 * - $created: Time the micro was published formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the micro. Increments each time it's output.
 *
 * micro status variables:
 * - $view_mode: View mode, e.g. 'full', 'teaser'...
 * - $teaser: Flag for the teaser state (shortcut for $view_mode == 'teaser').
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the micro.
 * - $readmore: Flags true if the teaser content of the micro cannot hold the
 *   main body content.
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
<div id="micro-<?php print $micro->mid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
<?php
/*
 *  <?php print $user_picture; ?>
 *
 *  <?php print render($title_prefix); ?>
 *  <?php if (!$page): ?>
 *    <h2<?php print $title_attributes; ?>><a href="<?php print $micro_url; ?>"><?php print $title; ?></a></h2>
 *  <?php endif; ?>
 *  <?php print render($title_suffix); ?>
 *
 *  <?php if ($display_submitted): ?>
 *    <div class="submitted">
 *      <?php
 *        print t('Submitted by !username on !datetime',
 *          array('!username' => $name, '!datetime' => $date));
 *      ?>
 *    </div>
 *  <?php endif; ?>
 *
 *  <div class="content"<?php print $content_attributes; ?>>
 *    <?php
 *      // We hide the comments and links now so that we can render them later.
 *      hide($content['comments']);
 *      hide($content['links']);
 *      print render($content);
 *    ?>
 *  </div>
 *
 *  <?php print render($content['links']); ?>
 *
 *  <?php print render($content['comments']); ?>
 */
 $all = get_defined_vars();
 dsm($all);
 dsm($variables);
 ?>

</div>
