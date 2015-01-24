<?php

/**
 * @file
 * Default theme implementation for the individual Slick item/slide template.
 *
 * Available variables:
 * - $attributes: An array of attributes to apply to the element.
 * - $item: A renderable array of the main image/background.
 * - $caption: A renderable array containing caption fields if provided:
 *   - Title: slide title.
 *   - Alt: core Image field Alt as caption.
 *   - Link: slide links or buttons.
 *   - Overlay: may be audio/video Media, or image.
 *   - Data: any possible field for more complex data if crazy enough.
 *
 * Notes:
 * .slide__pattern is also to fix draggable issue in Firefox, apart from
 * displaying overlay pattern. It is conditionally provided based on settings.
 * @see template_preprocess_slick_item()
 */
?>
<<?php print $tag; ?><?php print $attributes; ?>>
  <?php if ($settings['current_item'] == 'thumbnail'): ?>
    <?php print render($item); ?>
  <?php else: ?>

    <<?php print $content_tag; ?><?php print $content_attributes; ?>>

      <?php print render($item_prefix); ?>
      <?php print render($item); ?>
      <?php $slide_pattern && print $slide_pattern; ?>
      <?php print render($item_suffix); ?>

      <?php if (!empty($caption)): ?>
        <?php print render($title_prefix); ?>

        <div class="slide__caption">
          <?php if (isset($caption['overlay'])): ?>
            <div class="slide__overlay"><?php print render($caption['overlay']); ?></div>
          <?php endif; ?>

          <?php if (isset($caption['data']) || isset($caption['title'])): ?>
            <div class="slide__data">
              <?php if (!empty($caption['title'])): ?>
                <h2 class="slide__title"><?php print render($caption['title']); ?></h2>
              <?php endif; ?>

              <?php if (!empty($caption['alt'])): ?>
                <p class="slide__description"><?php print render($caption['alt']); ?></p>
              <?php endif; ?>

              <?php print render($caption['data']); ?>

              <?php if (isset($caption['link'])): ?>
                <div class="slide__link"><?php print render($caption['link']); ?></div>
              <?php endif; ?>
            </div>
          <?php endif; ?>
        </div>

        <?php print render($title_suffix); ?>
      <?php endif; ?>

      <?php isset($editor) && print render($editor); ?>

    </<?php print $content_tag; ?>>
  <?php endif; ?>
</<?php print $tag; ?>>
