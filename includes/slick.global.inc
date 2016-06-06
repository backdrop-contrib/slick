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
    'current_view_mode'   => '',
    'image_style'         => '',
    'media_switch'        => '',
    'optionset'           => 'default',
    'optionset_thumbnail' => '',
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
    'view_mode'           => '',
    'grid'                => '',
    'grid_medium'         => '',
    'grid_small'          => '',
    'visible_slides'      => '',
    'preserve_keys'       => FALSE,
    'override'            => FALSE,
    'overridables'        => array(),
    'cache'               => 0,
    'current_display'     => 'main',
  );
}

/**
 * Gets the image based on the Picture mapping, or Slick image lazy.
 */
function slick_get_image($settings = array(), &$media = array(), $key = 0, $media_settings = array()) {
  $image = array();

  // Ensures disabling Picture while being used doesn't screw up.
  $is_picture = function_exists('picture_mapping_load') && !empty($settings['picture_ready']);
  if ($is_picture && !empty($settings['picture'])) {
    $picture_style = empty($settings['picture_style']) ? 'large' : $settings['picture_style'];
    $fallback      = $settings['picture_fallback'];
    $mappings      = picture_mapping_load($picture_style);
    $breakpoints   = picture_get_mapping_breakpoints($mappings, $fallback);

    $slide = array(
      '#theme'       => 'picture',
      '#style_name'  => $fallback,
      '#breakpoints' => $breakpoints,
      '#attached'    => array(
        'library' => array(
          array('picture', 'picturefill'),
          array('picture', 'picture.ajax'),
        ),
      ),
    );

    foreach (array('uri', 'width', 'height', 'alt', 'title') as $data) {
      $slide["#$data"] = isset($media[$data]) ? $media[$data] : NULL;
    }
    $image = drupal_render($slide);
  }
  else {
    $item_attributes = array();
    $media['delta'] = $key;

    if (!empty($settings['image_style']) && !isset($media['_dimensions'])) {
      slick_get_dimensions($media, $settings['image_style']);
    }

    $item_attributes['height'] = $media['height'];
    $item_attributes['width']  = $media['width'];

    // @todo move to the container when Blazy CSS background landed.
    if (!empty($settings['thumbnail_style'])) {
      $item_attributes['data-thumb'] = image_style_url($settings['thumbnail_style'], $media['uri']);
    }

    // Blazy can lazyload a single image, Slick not, here comes the trouble.
    $lazy  = empty($settings['lazy']) || !empty($settings['unslick']) || $settings['count'] == 1 ? '' : $settings['lazy'];
    $blazy = !empty($settings['lazy']) && $settings['lazy'] == 'blazy';

    $image = array(
      '#theme'       => 'slick_image_lazy',
      '#item'        => $media,
      '#image_style' => $settings['image_style'],
      '#lazy'        => $blazy ? 'blazy' : $lazy,
      '#attributes'  => $item_attributes,
      '#options'         => array(
        'count'          => $settings['count'],
        'delta'          => $key,
        'has_pattern'    => !empty($settings['has_pattern']),
        'lazy_attribute' => $blazy ? 'src' : 'lazy',
        'lazy_class'     => $blazy ? 'b-lazy' : 'lazy',
        'type'           => empty($media['type']) ? 'image' : $media['type'],
        'url'            => array(),
      ),
    );

    if (!empty($settings['media_switch']) && $settings['media_switch'] != 'iframe') {
      slick_get_media_switch($image, $media, $settings);
    }
  }

  // Build the slide with picture, lightbox or multimedia supports.
  return array(
    '#theme'    => 'slick_media',
    '#item'     => $image,
    '#settings' => $media_settings,
    '#uri'      => $media['uri'],
  );
}

/**
 * Returns the media switch: colorbox/photobox, content, excluding iframe.
 */
function slick_get_media_switch(array &$image = array(), $media = array(), $settings = array()) {
  // Provide relevant file URL if it is a lightbox.
  if (strpos($settings['media_switch'], 'box') !== FALSE && !empty($settings['lightbox_ready'])) {
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
      $url = !empty($settings['box_style']) ? image_style_url($settings['box_style'], $media['uri']) : file_create_url($media['uri']);
    }

    $lightbox = str_replace('-switch', '', $settings['media_switch']);
    $classes  = array('slick__' . $lightbox, 'slick__litebox');
    if ($settings['media_switch'] == 'colorbox-switch') {
      if ($settings['count'] > 1) {
        $json['rel'] = $settings['id'];
      }
    }
    elseif ($settings['media_switch'] == 'photobox-switch') {
      if (!empty($media['url'])) {
        $image['#options']['url']['attributes']['rel'] = 'video';
      }
    }
    elseif ($settings['media_switch'] == 'slickbox-switch') {
      $classes = array('slick__box', 'slick__litebox');
      $json['entity_id'] = $settings['entity_id'];
      if (!empty($settings['entity_uri']) && !empty($settings['use_ajax'])) {
        $url = $settings['entity_uri'];
        $json['ajax'] = TRUE;
      }
    }

    // Video/audio has special attributes for JS interaction.
    if ($media['type'] != 'image') {
      $json['player'] = TRUE;
      $json['height'] = empty($media['media_height']) ? $media['height'] : $media['media_height'];
      $json['width']  = empty($media['media_width'])  ? $media['width']  : $media['media_width'];
      $image['#options']['url']['external'] = TRUE;
    }

    $image['#url'] = $url;
    $image['#options']['url']['attributes']['class'] = $classes;
    $image['#options']['url']['attributes']['data-media'] = drupal_json_encode($json);
    $image['#options']['lightbox'] = $lightbox;
  }
  elseif (strpos($settings['media_switch'], 'content') !== FALSE && !empty($settings['entity_uri'])) {
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
    'settings' => array(
      'count'        => $settings['count'],
      'has_pattern'  => !empty($settings['has_pattern']),
      'image_style'  => $settings['image_style'],
      'layout'       => empty($settings['slide_layout']) ? '' : $settings['slide_layout'],
      'media_switch' => empty($settings['media_switch']) ? '' : $settings['media_switch'],
      'ratio'        => !empty($settings['aspect_ratio']) && empty($settings['picture']) ? $settings['aspect_ratio'] : FALSE,
    ),
  );
  // $media['bundle'] = empty($settings['bundle']) ? '' : $settings['bundle'];
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
