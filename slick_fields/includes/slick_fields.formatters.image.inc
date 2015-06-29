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

  $build = array();
  foreach ($config['items'] as $key => $item) {
    $media = $slide = array();
    slick_extract_image_data($settings, $media, $slide, $item);

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
        '#uri'      => $media['uri'],
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
    $build[$key] = $slide;
  }
  return $build;
}
