<?php
/**
 * @file
 * Fields formatter for Slick and core Image field integration.
 */

/**
 * Format core image data.
 */
function _slick_fields_format_image(&$settings, $items, $field, $thumb_nav = FALSE) {
  $build = array();
  $css = array();

  $asnavfor = 'main';
  $slick_id = $settings['attributes']['id'];

  foreach ($items as $key => $item) {
    $slide = array(
      'item' => $item,
      'slide' => array(),
      'caption' => array(),
    );

    // Once media.module enabled, it populates type.
    $slide_type = isset($slide['item']['type']) ? $slide['item']['type'] : $field['type'];
    $settings['type'] = $slide_type;

    // Setup the variables.
    $image['uri']    = $slide['item']['uri'];
    $image['width']  = isset($slide['item']['width']) ? $slide['item']['width'] : NULL;
    $image['height'] = isset($slide['item']['height']) ? $slide['item']['height'] : NULL;
    $image['alt']    = $slide['item']['alt'];
    $image['title']  = $slide['item']['title'];

    $settings['media_height'] = $image['height'];
    $settings['media_width']  = $image['width'];

    $thumbnail = '';
    if ($thumbnail_style = $settings['thumbnail_style']) {
      $image['path'] = $image['uri'];
      $image['style_name'] = $thumbnail_style;
      $thumbnail_variables = $image;

      // Provides hidden static thumbnail revealed on dots hover, or custom
      // advanced work with thumbnail arrows.
      $thumbnail_url = image_style_url($thumbnail_style, $image['uri']);
      $thumbnail_variables['attributes']['data-thumb'] = $thumbnail_url;
      $image['attributes']['data-thumb'] = $thumbnail_url;

      $thumbnail = theme('image_style', $thumbnail_variables);
    }

    if (!$thumb_nav) {
      // Slide captions.
      if ($settings['slide_caption']) {
        if ($image['title']) {
          $slide['caption']['title']['#markup'] = filter_xss($image['title']);
        }
        if ($image['alt']) {
          $slide['caption']['alt']['#markup'] = filter_xss($image['alt']);
        }
      }

      // Slide layouts.
      if ($settings['slide_layout']) {
        $slide['caption']['layout'] = check_plain($settings['slide_layout']);
      }

      // Image with picture and colorbox supports.
      $image_slide = _slick_fields_get_picture($settings, $image, $key);
    }
    else {
      // Thumbnail slider if so configured.
      $image_slide = $thumbnail;
      $settings['type'] = 'image';
      $asnavfor = 'thumbnail';

      // Thumbnail captions.
      if ($settings['thumbnail_caption']) {
        $thumbnail_caption = '';
        // Sanitize once below.
        if ($settings['thumbnail_caption'] == 'title_field' && $image['title']) {
          $thumbnail_caption = $image['title'];
        }
        elseif ($settings['thumbnail_caption'] == 'alt_field' && $image['alt']) {
          $thumbnail_caption = $image['alt'];
        }
        if ($thumbnail_caption) {
          $slide['caption']['data']['#markup'] = filter_xss($thumbnail_caption);
        }
      }
    }

    // Pass it over to theme_slick_media.
    $slide['slide'] = array(
      '#theme' => 'slick_media',
      '#item' => $image_slide,
      '#settings' => $settings,
      '#asnavfor' => $asnavfor,
    );

    // Use slick_inline_css_output_info_alter() to modify the outputs.
    $inline_css_skins = slick_inline_css_skins();
    if (in_array($settings['skin'], $inline_css_skins)) {
      $slide['slide'] = array();
      $fullscreen_image_url = image_style_url($settings['image_style'], $image['uri']);
      $css[] = "#{$slick_id} .slide--{$key} {background-image: url('{$fullscreen_image_url}')}";
    }

    $build[$key] = $slide;
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }

  return $build;
}
