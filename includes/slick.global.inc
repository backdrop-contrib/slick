<?php

/**
 * @file
 * Global functions across Slick field formatters and Views.
 */

/**
 * Defines global field formatter and Views settings.
 */
function slick_get_global_default_settings() {
  return array(
    'aspect_ratio'        => '',
    'background'          => FALSE,
    'cache'               => 0,
    'current_display'     => 'main',
    'current_view_mode'   => '',
    'image_style'         => '',
    'media_switch'        => '',
    'mousewheel'          => FALSE,
    'optionset'           => 'default',
    'optionset_thumbnail' => '',
    'override'            => FALSE,
    'overridables'        => array(),
    'skin'                => '',
    'skin_thumbnail'      => '',
    'skin_arrows'         => '',
    'skin_dots'           => '',
    'slide_caption'       => array(),
    'slide_classes'       => '',
    'slide_image'         => '',
    'slide_layout'        => '',
    'slide_overlay'       => array(),
    'slide_link'          => '',
    'slide_title'         => '',
    'thumbnail_caption'   => '',
    'thumbnail_effect'    => '',
    'view_mode'           => '',
    'grid'                => '',
    'grid_medium'         => '',
    'grid_small'          => '',
    'visible_slides'      => '',
    'preserve_keys'       => FALSE,
    'type'                => '',
  );
}

/**
 * Gets the image based on the Picture mapping, or Slick image lazy.
 */
function slick_get_image($settings = array(), &$media = array(), $key = 0, $slide_settings = array()) {
  // @todo simplify arguments at Slick 3.x like everything else into settings.
  $media['delta'] = $key;
  $settings = array_merge($settings, $media);

  $image = array(
    '#theme'       => 'slick_image',
    '#image_style' => $settings['image_style'],
    '#url'         => '',
    '#uri'         => $settings['uri'],
  );

  // Ensures disabling Picture while being used doesn't screw up.
  $item_attributes = array();
  $is_picture = function_exists('picture_mapping_load') && !empty($settings['picture_ready']);
  if ($is_picture && !empty($settings['picture'])) {
    $picture_style = empty($settings['picture_style']) ? 'large' : $settings['picture_style'];
    $fallback      = $settings['picture_fallback'];
    $mappings      = picture_mapping_load($picture_style);
    $breakpoints   = picture_get_mapping_breakpoints($mappings, $fallback);

    $item_attributes['breakpoints'] = $breakpoints;
    $item_attributes['style_name']  = $fallback;
  }
  else {
    $item_attributes['height'] = $media['height'];
    $item_attributes['width']  = $media['width'];

    // Blazy can lazyload a single image, Slick not, yet, here comes the trouble.
    if (!empty($settings['blazy'])) {
      $settings['lazy_attribute'] = 'src';
      $settings['lazy_class'] = 'b-lazy';
    }
    elseif (!empty($settings['lazy'])) {
      $settings['lazy_attribute'] = $settings['lazy_class'] = 'lazy';
    }
  }

  // Now that picture is inside theme_slick_image(), we can manipulate it.
  if (!empty($settings['media_switch']) && $settings['media_switch'] != 'iframe-switch') {
    slick_get_media_switch($image, $settings);
  }

  // Build the slide with picture, lightbox or multimedia supports.
  $image['#item'] = $media;
  $image['#item_attributes'] = $item_attributes;

  // @todo remove $slide_settings when merged, and fully tested.
  $image['#settings'] = array_merge($settings, $slide_settings);

  return $image;
}

/**
 * Returns the media switch: colorbox/photobox, content, excluding iframe.
 */
function slick_get_media_switch(&$image = array(), $settings = array()) {
  $switch = str_replace('-switch', '', $settings['media_switch']);

  // Provide relevant file URL if it is a lightbox.
  if (!empty($settings['lightbox']) && !empty($settings['lightbox_ready'])) {
    $json = array('type' => $settings['type']);
    if (!empty($settings['embed_url'])) {
      $url = $settings['embed_url'];
      $json['scheme'] = $settings['scheme'];
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
      $url = !empty($settings['box_style']) ? image_style_url($settings['box_style'], $settings['uri']) : file_create_url($settings['uri']);
    }

    $classes  = array('slick__' . $switch, 'slick__litebox');
    if ($switch == 'colorbox' && $settings['count'] > 1) {
      $json['rel'] = $settings['id'];
    }
    elseif ($switch == 'photobox' && !empty($settings['embed_url'])) {
      $image['#url_attributes']['rel'] = 'video';
    }
    elseif ($switch == 'slickbox') {
      $classes = array('slick__box', 'slick__litebox');
      $json['entity_id'] = $settings['entity_id'];
      if (!empty($settings['entity_uri']) && !empty($settings['use_ajax'])) {
        $url = $settings['entity_uri'];
        $json['ajax'] = TRUE;
      }
    }

    // Video/audio has special attributes for JS interaction.
    if ($settings['type'] != 'image') {
      $json['player'] = TRUE;
      $json['height'] = empty($settings['box_height']) ? $settings['height'] : $settings['box_height'];
      $json['width']  = empty($settings['box_width'])  ? $settings['width']  : $settings['box_width'];
    }

    $image['#url'] = $url;
    $image['#url_attributes']['class'] = $classes;
    $image['#url_attributes']['data-media'] = drupal_json_encode($json);
  }
  elseif ($switch == 'content' && !empty($settings['entity_uri'])) {
    $image['#url'] = $settings['entity_uri'];
  }
}

/**
 * Extracts the needed image data.
 */
function slick_extract_image_data($settings, array &$media, array &$slide, $item = array()) {
  $slide += array(
    'item'     => $item,
    'slide'    => array(),
    'caption'  => array(),
    'settings' => $settings,
  );

  if (!empty($settings['image_style']) && !isset($media['_dimensions'])) {
    slick_get_dimensions($media, $settings['image_style']);
  }

  foreach (array('fid', 'alt', 'title', 'uri', 'type') as $data) {
    $media[$data] = isset($item[$data]) ? $item[$data] : NULL;
  }

  foreach (array('height', 'width') as $data) {
    $media[$data] = !empty($media[$data]) ? $media[$data] : (isset($item[$data]) ? $item[$data] : NULL);
  }
}

/**
 * Returns items as a grid display.
 */
function slick_build_grids(array $build, &$settings) {
  $grids = array();

  // Display all items if unslick is enforced for plain grid to lightbox.
  if (!empty($settings['unslick'])) {
    $settings['current_display'] = 'main';
    $settings['current_item']    = 'grid';
    $settings['count']           = 2;

    // Displays all items if unslick is enforced such as plain grid to lightbox.
    $slide['slide'] = array(
      '#theme'    => 'slick_grid',
      '#items'    => $build,
      '#delta'    => 0,
      '#settings' => $settings,
    );
    $slide['settings'] = $settings;
    $grids[0] = $slide;
  }
  else {
    // Otherwise do chunks to have a grid carousel.
    $preserve_keys     = !empty($settings['preserve_keys']);
    $grid_items        = array_chunk($build, $settings['visible_slides'], $preserve_keys);
    $settings['count'] = count($grid_items);

    foreach ($grid_items as $delta => $grid_item) {
      $slide = array();
      $slide['slide'] = array(
        '#theme'    => 'slick_grid',
        '#items'    => $grid_item,
        '#delta'    => $delta,
        '#settings' => $settings,
      );
      $slide['settings'] = $settings;
      $grids[] = $slide;
      unset($slide);
    }
  }

  return $grids;
}

/**
 * Defines image style dimensions once for all images as it costs a bit.
 */
function slick_get_dimensions(array &$image = array(), $image_style) {
  $dimensions = array(
    'width'  => isset($image['width'])  ? $image['width']  : NULL,
    'height' => isset($image['height']) ? $image['height'] : NULL,
  );

  image_style_transform_dimensions($image_style, $dimensions);
  $image['width']  = $dimensions['width'];
  $image['height'] = $dimensions['height'];
  $image['_dimensions'] = TRUE;
}

/**
 * Returns slick instances from the cache with simplified renderable.
 */
function slick_render_cache($settings = array()) {
  $cached = FALSE;
  $cid = $settings['id'] . ':' . $settings['cid'];
  if ($cache = cache_get($cid)) {
    $cached = $settings['cache'] == 'persistent' ? TRUE : REQUEST_TIME < $cache->expire;
  }

  if ($cached) {
    $slick[0] = $cache->data;
    if ($settings['nav'] && $thumb_cache = cache_get($cid . ':thumbnail')) {
      $slick[1] = $thumb_cache->data;
    }
    return $slick;
  }

  return $cached;
}
