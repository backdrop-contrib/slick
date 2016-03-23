<?php

/**
 * @file
 * Fields formatter for Slick and core Image field integration.
 */

/**
 * Formats core image data.
 */
function slick_format_image($items, $langcode, array &$settings) {
  $settings['picture_ready'] = $settings['lightbox_ready'] = TRUE;

  $build = $media = array();

  foreach ($items as $key => $item) {
    $slide = array();
    $item['type'] = !isset($item['type']) ? 'image' : $item['type'];
    slick_extract_image_data($settings, $media, $slide, $item);

    if (!empty($settings['slide_caption'])) {
      foreach ($settings['slide_caption'] as $caption) {
        $data = str_replace("_field", "", $caption);
        if (!empty($media[$data])) {
          $slide['caption'][$data]['#markup'] = filter_xss($media[$data]);
        }
      }
    }

    // Image with picture, lazyLoad, and lightbox supports.
    $slide['slide'] = slick_get_image($settings, $media, $key, $slide['settings']);
    $build['items'][$key] = $slide;

    if (!empty($settings['nav'])) {
      // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
      $slide['slide']   = slick_get_thumbnail($settings['thumbnail_style'], $media);
      $slide['caption'] = array();
      if (!empty($settings['thumbnail_caption'])) {
        $data = str_replace("_field", "", $settings['thumbnail_caption']);
        if (!empty($media[$data])) {
          $slide['caption']['data']['#markup'] = filter_xss($media[$data]);
        }
      }
      $build['thumb'][$key] = $slide;
    }
    unset($slide);
  }
  return $build;
}
