<?php
// $Id: micro.tpl.php,v 1.18 2010/01/07 05:23:51 webchick Exp $

/**
 * @file
 * Default theme implementation for micros.
 *
 * Available variables:
 * - $author: micro author. Can be link or plain text.
 * - $content: An array of micro items. Use render($content) to print them all, or
 *   print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $created: Formatted date and time for when the micro was created.
 *   Preprocess functions can reformat it by calling format_date() with the
 *   desired parameters on the $micro->created variable.
 * - $changed: Formatted date and time for when the micro was last changed.
 *   Preprocess functions can reformat it by calling format_date() with the
 *   desired parameters on the $micro->changed variable.
 * - $new: New micro marker.
 * - $permalink: micro permalink.
 * - $picture: Authors picture.
 * - $signature: Authors signature.
 * - $status: micro status. Possible values are:
 *   micro-unpublished, micro-published or micro-preview.
 * - $title: Linked title.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the following:
 *   - micro: The current template type, i.e., "theming hook".
 *   - micro-by-anonymous: micro by an unregistered user.
 *   - micro-by-node-author: micro by the author of the parent node.
 *   - micro-preview: When previewing a new or edited micro.
 *   The following applies only to viewers who are registered users:
 *   - micro-unpublished: An unpublished micro visible only to administrators.
 *   - micro-by-viewer: micro by the user currently viewing the page.
 *   - micro-new: New micro since last the visit.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * These two variables are provided for context:
 * - $micro: Full micro object.
 * - $node: Node object the micros are attached to.
 *
 * Other variables:
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess()
 * @see template_preprocess_micro()
 * @see template_process()
 * @see theme_micro()
 */
?>
<?php
/*
 *<div class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
 *  <?php print $picture ?>
 *
 *  <?php if ($new): ?>
 *    <span class="new"><?php print $new ?></span>
 *  <?php endif; ?>
 *
 *  <?php print render($title_prefix); ?>
 *  <h3<?php print $title_attributes; ?>><?php print $title ?></h3>
 *  <?php print render($title_suffix); ?>
 *
 *  <div class="submitted">
 *    <?php print $permalink; ?>
 *    <?php
 *      print t('Submitted by !username on !datetime.',
 *        array('!username' => $author, '!datetime' => $created));
 *    ?>
 *  </div>
 *
 *  <div class="content"<?php print $content_attributes; ?>>
 *    <?php
 *      // We hide the comments and links now so that we can render them later.
 *      hide($content['links']);
 *      print render($content);
 *    ?>
 *    <?php if ($signature): ?>
 *    <div class="user-signature clearfix">
 *      <?php print $signature ?>
 *    </div>
 *    <?php endif; ?>
 *  </div>
 *
 *  <?php print render($content['links']) ?>
 *</div>
 */
 print render($content);
  $all = get_defined_vars();
  dsm($all);
 ?>
