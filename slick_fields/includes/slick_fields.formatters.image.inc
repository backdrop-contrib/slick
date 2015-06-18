<?php
/**
 * @file
 * Fields formatter for Slick and core Image field integration.
 */

/**
 * Formats core image data.
 */
function _slick_fields_format_image(&$settings, $config, $thumb_nav = FALSE) {
  $current_display           = 'main';
  $slick_id                  = $settings['attributes']['id'];
  $settings['picture_ready'] = $settings['lightbox_ready'] = TRUE;
  $settings['type']          = 'image';

  $build = $css = array();
  foreach ($config['items'] as $key => $item) {
    $slide = array(
      'item'     => $item,
      'caption'  => array(),
      'settings' => array(
        'count'        => $settings['count'],
        'layout'       => $settings['slide_layout'],
        'media_switch' => $settings['media_switch'],
        'has_pattern'  => $settings['has_pattern'],
      ),
    );

    foreach ($config['data'] as $data) {
      $image[$data] = $slide['settings'][$data] = isset($item[$data]) ? $item[$data] : NULL;
      foreach (array('width', 'height') as $data) {
        $image[$data] = isset($item['metadata'][$data]) ? $item['metadata'][$data] : (isset($item[$data]) ? $item[$data] : NULL);
      }
    }

    // Clone image info to individual theme_slick_media() settings.
    foreach ($image as $i => $value) {
      $slide['settings'][$i] = $image[$i];
    }
    $slide['settings']['type'] = empty($slide['settings']['type']) ? $settings['type'] : $slide['settings']['type'];

    $thumbnail = '';
    if ($thumbnail_style = $settings['thumbnail_style']) {
      $thumb               = $image;
      $thumb['path']       = $image['uri'];
      $thumb['style_name'] = $thumbnail_style;

      // Provides hidden static thumbnail revealed on dots hover.
      $image['attributes']['data-thumb'] = image_style_url($thumbnail_style, $image['uri']);
      $thumbnail = theme('image_style', $thumb);
    }

    if (!$thumb_nav) {
      if ($settings['slide_caption']) {
        foreach (array('alt', 'title') as $caption) {
          if ($image[$caption]) {
            $slide['caption'][$caption]['#markup'] = filter_xss($image[$caption]);
          }
        }
      }

      // Image with picture and lightbox supports.
      $image_slide = slick_get_image($settings, $slide['settings'], $image, $key);
    }
    else {
      $image_slide     = $thumbnail;
      $current_display = 'thumbnail';

      // Thumbnail captions.
      if ($caption = $settings['thumbnail_caption']) {
        $thumbnail_caption = '';
        if ($caption == 'title_field' && $image['title']) {
          $thumbnail_caption = $image['title'];
        }
        elseif ($caption == 'alt_field' && $image['alt']) {
          $thumbnail_caption = $image['alt'];
        }
        if ($thumbnail_caption) {
          $slide['caption']['data']['#markup'] = filter_xss($thumbnail_caption);
          $slide['settings']['type'] = empty($thumbnail) ? 'text' : 'image';
        }
      }
    }

    // Pass it over to theme_slick_media().
    $slide['slide'] = array(
      '#theme'    => 'slick_media',
      '#item'     => $image_slide,
      '#settings' => $slide['settings'],
      '#display'  => $current_display,
    );

    // Fullscreen skin needs inline CSS if so configured.
    if ($config['inline_css']) {
      $slide['slide'] = array();
      $image_url      = $settings['image_style'] ? image_style_url($settings['image_style'], $image['uri']) : file_create_url($image['uri']);
      $css[]          = "#{$slick_id} .slide--{$key} {background-image: url('{$image_url}')}";
    }

    $build[$key] = $slide;
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }

  return $build;
}
