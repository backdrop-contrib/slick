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
 *  - $url: The full url including query options for the iframes.
 *  - $alternative_content: Text to display for browsers that don't support
 *      iframes.
 *  - $settings: An array containing cherry-picked settings.
 */
?>
<?php if ($asnavfor == 'thumbnail'): ?>
  <?php print render($item); ?>

<?php
  // Main media, er, this silly line is to satisfy pareview.sh till the fix.
  else: ?>

  <div<?php print $attributes; ?>>
    <?php if (in_array($settings['type'], array('video', 'audio'))): ?>
      <?php if (!$is_lightbox): ?>
        <iframe<?php print $content_attributes; ?> allowfullscreen><?php print $alternative_content; ?></iframe>
      <?php endif; ?>
      <?php if ($settings['media_switch'] == 'iframe-switch'): ?>
        <i class="media-icon media-icon--close"></i>
        <i class="media-icon media-icon--play"></i>
        <i class="media-icon media-icon--spinner"></i>
      <?php endif; ?>
    <?php endif; ?>

    <?php if ($item): ?>
      <?php print render($item_prefix); ?>
      <?php print render($item); ?>
      <?php print render($item_suffix); ?>
    <?php endif; ?>
  </div>
<?php endif; ?>
