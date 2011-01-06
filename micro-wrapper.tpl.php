<?php
// $Id$

/**
 * @file
 * Default theme implementation for micros.
 *
 * Available variables:
 * - $content: An array of contextual information.
 * - $items: An array of micro items. Use render($items) to print them all.
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $entity: Entity object the micro items are attached to.
 * - $eid: Entity ID to which the micro items are attached to.
 * - $view_mode: The parent entity's view mode.
 * - $bundle: The bundle object of the micro items.
 * - $type: The name of the micro type bundle.
 * - $classes: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess()
 * @see template_preprocess_micro()
 * @see template_process()
 * @see theme_micro()
 */
?>
<div id="micro" class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php if (!empty($items)): ?>
    <?php print render($title_prefix); ?>
    <h2 class="title"><?php print t('%type', array('%type' => $type)); ?></h2>
    <?php print render($title_suffix); ?>
  <?php endif; ?>

  <?php print render($items); ?>
  <?php if ($form['#theme']): ?>
    <h2 class="title micro-form"><?php print t('Add new %type', array('%type' => strtolower($type))); ?></h2>
    <?php print render($form); ?>
  <?php endif; ?>
</div>
