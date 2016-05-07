<?php
/**
 * @file
 * Default theme implementation for the Slick media template.
 *
 * The Slick media support core image, vimeo, youtube and soundcloud.
 * If iframe switcher is enabled, audio/video iframe will be hidden below image
 * overlay, and only visible when toggled.
 *
 * Available variables:
 *  - $settings: An array containing cherry-picked settings.
 */
?>
<div<?php print $attributes; ?>>
<?php print render($item); ?>
<?php if ($settings['is_media']): ?>
  <?php if (!$is_lightbox): ?>
    <iframe<?php print $content_attributes; ?> allowfullscreen></iframe>
  <?php endif; ?>
  <?php if ($settings['media_switch'] == 'iframe-switch'): ?>
    <i class="media-icon media-icon--close"></i>
    <i class="media-icon media-icon--play"></i>
    <i class="media-icon media-icon--spinner"></i>
  <?php endif; ?>
<?php endif; ?>
</div>
