<?php
/**
 * @file
 * Fields formatter for the Slick and Media integration.
 */

/**
 * Formats image/media file data.
 */
function slick_format_media($items, $langcode, array &$settings) {
  $settings['lightbox_ready'] = $settings['picture_ready'] = TRUE;

  $build = $dimensions = $thumbnails = array();

  // Defines image dimensions once to reduce function calls.
  if (!empty($settings['image_style'])) {
    $image = array();
    $dimensions = slick_get_dimensions($image, $settings['image_style']);
  }

  // Build the main display.
  $overlays = !empty($settings['slide_overlay']);
  $layouts  = !empty($settings['layout']);
  $classes  = !empty($settings['slide_classes']);
  foreach ($items as $key => $item) {
    $file  = (object) $item;
    $slide = array();
    $media = $dimensions;

    $settings['delta'] = $key;

    // Needed settings: type scheme image_style view_mode.
    // Extracts media data for: embed_url, box_height, box_width, image uri.
    slick_extract_media_data($settings, $media, $file);

    // Build the fieldable captions.
    slick_get_caption($settings, 'file', $file, $slide);

    // Plain text video URL for plain iframes as video overlay over image.
    if ($overlays && $overlay = field_get_items('file', $file, $settings['slide_overlay'])) {
      $url = strip_tags($overlay[0]['value']);
      if ($media_id = slick_get_media_id($url)) {
        $slide['caption']['overlay'] = slick_get_media_overlay($url, $media_id);
      }
    }

    // Layout field as opposed to the builtin layout options.
    if ($layouts && strpos($settings['layout'], 'field_') !== FALSE) {
      if ($layout = field_get_items('file', $file, $settings['layout'])) {
        $settings['layout'] = strip_tags($layout[0]['value']);
      }
    }

    // Slide classes, if so configured.
    if ($classes && $classes = field_get_items('file', $file, $settings['slide_classes'])) {
      $settings['slide_classes'] = strip_tags($classes[0]['value']);
    }

    // Image with picture, lazyLoad, and lightbox supports.
    $slide['slide'] = slick_get_image($settings, $media, $item);
    $slide['settings'] = $settings;

    $thumbnails[$key] = $media;

    // Build the slide.
    $build['items'][$key] = $slide;
    unset($file, $slide);
  }

  // Build the thumbnail display.
  if (!empty($settings['nav'])) {
    $captions = !empty($settings['thumbnail_caption']);
    foreach ($items as $key => $item) {
      $file  = (object) $item;
      $media = $thumbnails[$key];

      // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
      $slide['slide'] = slick_get_thumbnail($settings['thumbnail_style'], $media);

      if ($captions && $caption = field_get_items('file', $file, $settings['thumbnail_caption'])) {
        $slide['caption']['data']['#markup'] = filter_xss($caption[0]['value']);
      }

      $build['thumb'][$key] = $slide;
      unset($file, $slide);
    }
  }

  return $build;
}
