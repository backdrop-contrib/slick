<?php
/**
 * @file
 * Fields formatter for Slick and core Image field integration.
 */

/**
 * Formats core image data.
 */
function _slick_fields_format_image(&$settings, $config, $thumb_nav = FALSE) {
  $settings['picture_ready'] = $settings['lightbox_ready'] = TRUE;

  $build = $css = $media = array();
  foreach ($config['items'] as $key => $item) {
    $slide = array(
      'item'     => $item,
      'slide'    => array(),
      'settings' => array(
        'count'        => $settings['count'],
        'has_pattern'  => $settings['has_pattern'],
        'image_style'  => $settings['image_style'],
        'layout'       => $settings['slide_layout'],
        'media_switch' => $settings['media_switch'],
      ),
    );
    slick_extract_image_data($media, $item);

    // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
    $thumb = empty($settings['thumbnail_style']) ? array() : slick_get_thumbnail($settings['thumbnail_style'], $media);
    if (!$thumb_nav) {
      if (!empty($settings['slide_caption'])) {
        foreach ($settings['slide_caption'] as $caption) {
          $data = str_replace("_field", "", $caption);
          if (!empty($media[$data])) {
            $slide['caption'][$data]['#markup'] = filter_xss($media[$data]);
          }
        }
      }

      // Image with picture, lazyLoad, and lightbox supports.
      $slide['slide'] = array(
        '#theme'    => 'slick_media',
        '#item'     => slick_get_image($settings, $media, $key),
        '#settings' => $slide['settings'],
      );
    }
    else {
      $slide['slide'] = $thumb;
      if (!empty($settings['thumbnail_caption'])) {
        $data = str_replace("_field", "", $settings['thumbnail_caption']);
        if (!empty($media[$data])) {
          $slide['caption']['data']['#markup'] = filter_xss($media[$data]);
        }
      }
    }

    // Fullscreen skin needs inline CSS if so configured.
    if ($config['inline_css']) {
      $css[] = slick_get_inline_css($settings, $slide, $key, $media['uri']);
    }
    $build[$key] = $slide;
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }
  return $build;
}
