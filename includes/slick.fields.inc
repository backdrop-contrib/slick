<?php
/**
 * @file
 * Global functions for field formatter.
 */

/**
 * Defines default field formatter settings to avoid notices.
 */
function slick_get_fields_default_settings() {
  $settings = array(
    'optionset'            => 'default',
    'optionset_thumbnail'  => '',
    'asnavfor_main'        => '',
    'asnavfor_thumbnail'   => '',
    'asnavfor_auto'        => FALSE,
    'skin'                 => '',
    'skin_thumbnail'       => '',
    'skin_arrows'          => '',
    'skin_dots'            => '',
    'slide_layout'         => '',
    'image_style'          => '',
    'thumbnail_style'      => '',
    'thumbnail_hover'      => FALSE,
    'thumbnail_caption'    => '',
    'slide_image'          => '',
    'slide_caption'        => array(),
    'media_switch'         => '',
    'mousewheel'           => FALSE,
    'override'             => FALSE,
    'overridables'         => array(),
    'slide_classes'        => '',
    'color_field'          => '',
    'nested_slick'         => FALSE,
    'nested_optionset'     => '',
    'nested_style'         => '',
    'slide_overlay'        => array(),
    'slide_link'           => '',
    'slide_title'          => '',
    'view_mode'            => '',
    'aspect_ratio'         => '',
    'picture'              => FALSE,
    'picture_style'        => '',
    'picture_fallback'     => '',
    'colorbox_style'       => '',
    'markup'               => FALSE,
    'type'                 => '',
    'lightbox_ready'       => FALSE,
    'picture_ready'        => FALSE,
    'current_display'      => 'main',
  );

  drupal_alter('slick_fields_default_settings_info', $settings);
  return $settings;
}

/**
 * Gets the rendered image based on the Picture mapping, or Slick image lazy.
 */
function slick_get_image($settings = array(), $media = array(), $key = 0) {
  $build = array();

  // Must be similar for the colorbox rel gallery for image/video/audio.
  $id = isset($settings['lightbox_id']) ? $settings['lightbox_id'] : $settings['attributes']['id'];

  // Ensures disabling Picture while being used doesn't screw up.
  $is_picture = function_exists('picture_mapping_load') && !empty($settings['picture_ready']);
  if ($is_picture && $settings['picture'] && !empty($settings['picture_style'])) {
    $fallback    = $settings['picture_fallback'];
    $mappings    = picture_mapping_load($settings['picture_style']);
    $breakpoints = picture_get_mapping_breakpoints($mappings, $fallback);

    $slide = array(
      '#theme' => 'picture',
      '#style_name' => $settings['picture_style'],
      '#breakpoints' => $breakpoints,
      '#attached' => array(
        'library' => array(
          array('picture', 'picturefill'),
          array('picture', 'picture.ajax'),
        ),
      ),
    );

    foreach (array('uri', 'width', 'height', 'alt', 'title') as $data) {
      $slide["#$data"] = isset($media[$data]) ? $media[$data] : NULL;
    }
    $build = render($slide);
  }
  else {
    if (!empty($settings['thumbnail_style'])) {
      $media['attributes']['data-thumb'] = image_style_url($settings['thumbnail_style'], $media['uri']);
    }
    $build = array(
      '#theme' => 'slick_image_lazy',
      '#item' => $media,
      '#image_style' => $settings['image_style'],
      '#lazy' => empty($settings['lazy']) ? '' : $settings['lazy'],
      '#options' => array(
        'type' => $media['type'],
        'has_pattern' => !empty($settings['has_pattern']),
        'count' => $settings['count'],
        'delta' => $key,
        'url' => array(),
      ),
    );

    // Provide relevant link path if it is a lightbox.
    if (strpos($settings['media_switch'], 'box') !== FALSE && $settings['lightbox_ready']) {
      $json = array('type' => $media['type']);
      if (!empty($media['url'])) {
        $url = $media['url'];
        $json['scheme'] = $media['scheme'];
        // Force autoplay for media URL on lightboxes, saving another click.
        if ($json['scheme'] == 'soundcloud') {
          if (strpos($url, 'auto_play') === FALSE || strpos($url, 'auto_play=false') !== FALSE) {
            $url = strpos($url, '?') === FALSE ? $url . '?auto_play=true' : $url . '&amp;auto_play=true';
          }
        }
        elseif (strpos($url, 'autoplay') === FALSE || strpos($url, 'autoplay=0') !== FALSE) {
          $url = strpos($url, '?') === FALSE ? $url . '?autoplay=1' : $url . '&amp;autoplay=1';
        }
      }
      else {
        $url = !empty($settings['colorbox_style']) ? image_style_url($settings['colorbox_style'], $media['uri']) : file_create_url($media['uri']);
      }

      if ($settings['media_switch'] == 'colorbox-switch') {
        $classes = array('slick__colorbox', 'slick__litebox');
        if ($settings['count'] > 1) {
          $json['rel'] = $id;
        }
      }
      elseif ($settings['media_switch'] == 'photobox-switch') {
        $classes = array('slick__photobox', 'slick__litebox');
        if (!empty($media['url'])) {
          $build['#options']['url']['attributes']['rel'] = 'video';
        }
        if (!empty($settings['thumbnail_style'])) {
          $build['#item']['attributes']['data-src'] = image_style_url($settings['thumbnail_style'], $media['uri']);
        }
      }

      // Video/audio has special attributes for JS interaction.
      if ($media['type'] != 'image') {
        $json['player'] = TRUE;
        $json['height'] = $media['height'];
        $json['width'] = $media['width'];
        $build['#options']['url']['external'] = TRUE;
      }

      $build['#url'] = $url;
      $build['#options']['url']['attributes']['class'] = $classes;
      $build['#options']['url']['attributes']['data-media'] = drupal_json_encode($json);
      $build['#options']['lightbox'] = str_replace('-switch', '', $settings['media_switch']);
    }
  }
  drupal_alter('slick_image_info', $build, $settings, $media, $key);
  return $build;
}

/**
 * Gets the thumbnail image.
 */
function slick_get_thumbnail($thumbnail_style = '', $media = array()) {
  $thumbnail = array();
  if (!empty($thumbnail_style)) {
    $thumbnail = array(
      '#theme' => 'image_style',
      '#style_name' => $thumbnail_style,
      '#path' => $media['uri'],
    );
    foreach (array('alt', 'height', 'title', 'width') as $data) {
      $thumbnail["#$data"] = isset($media[$data]) ? $media[$data] : NULL;
    }
  }
  return $thumbnail;
}

/**
 * Extracts the needed image data.
 */
function slick_extract_image_data($settings, array &$media, array &$slide, $item = array()) {
  $slide += array(
    'item'     => $item,
    'slide'    => array(),
    'caption'  => array(),
    'settings' => array(
      'count'        => $settings['count'],
      'has_pattern'  => !empty($settings['has_pattern']),
      'image_style'  => $settings['image_style'],
      'layout'       => !empty($settings['slide_layout']) ? $settings['slide_layout'] : '',
      'media_switch' => !empty($settings['media_switch']) ? $settings['media_switch'] : '',
    ),
  );

  $config = array('fid', 'height', 'width', 'alt', 'title', 'uri', 'type');
  foreach ($config as $data) {
    $media[$data] = isset($item[$data]) ? $item[$data] : NULL;
  }
}

/**
 * Checks whether a skin expecting inline CSS background, not images.
 */
function slick_get_inline_css_skins($skin = NULL) {
  $inline_css = &drupal_static(__FUNCTION__, NULL);
  if (!isset($inline_css)) {
    $skins = slick_skins();
    $inline_css = empty($skin) ? FALSE : isset($skins[$skin]['inline css']) && $skins[$skin]['inline css'];
  }
  return $inline_css;
}

/**
 * Builds the inline CSS output for skins with explicit 'inline css' key.
 */
function slick_get_inline_css(array $settings, array &$items) {
  $css = array();
  if (slick_get_inline_css_skins($settings['skin'])) {
    $id = $settings['attributes']['id'];
    foreach ($items as $key => &$item) {
      $slick = $settings['count'] > 1 ? $id . " .slide--{$key}" : $id;
      if (isset($item['slide']['#uri']) && $uri = $item['slide']['#uri']) {
        $image_url = !empty($settings['image_style']) ? image_style_url($settings['image_style'], $uri) : file_create_url($uri);
        $css[] = "#{$slick} {background-image: url('{$image_url}')}";
        $item['slide'] = array();
      }
    }
  }

  // Or individual dynamic slide colors as offered by FC.
  if (!empty($settings['inline_css'])) {
    $css = array_merge($css, $settings['inline_css']);
    unset($settings['inline_css']);
  }

  if ($css) {
    $css = implode("\n", $css);
    drupal_alter('slick_inline_css_info', $css, $items, $settings);
  }
  return $css;
}
