<?php

/**
 * @file
 * Fields formatter for the Slick and Field collection integration.
 */

/**
 * Formats Field collection data.
 */
function slick_format_field_collection($items, $langcode, array &$settings) {
  $view_mode   = $settings['view_mode'] ? $settings['view_mode'] : 'full';
  $slick_id    = $settings['id'];
  $nested_name = empty($settings['nested_optionset']) ? 'default' : $settings['nested_optionset'];
  $nested_set  = empty($settings['nested_slick']) ? NULL : slick_optionset_load($nested_name);
  $entity_type = 'field_collection_item';
  // It is un-reasonable to make large image as colorbox, so disable it.
  $settings['lightbox_ready'] = FALSE;
  $settings['picture_ready']  = TRUE;

  $build = array();
  foreach ($items as $key => $item) {
    $slide   = $thumb = array();
    $fc      = field_collection_field_get_entity($item);
    $wrapper = entity_metadata_wrapper($entity_type, $fc);
    $view    = $fc->view($view_mode);
    $id      = array_keys($view[$entity_type]);
    $id      = reset($id);
    $fields  = $view[$entity_type][$id];

    // Main background image can be core Image, or File entity, multi or single.
    if (!empty($settings['slide_image']) && $slide_bg = $wrapper->{$settings['slide_image']}->value()) {
      $file  = isset($slide_bg['fid']) ? $slide_bg : (isset($slide_bg[0]) ? $slide_bg[0] : NULL);
      $media = array(
        'scheme'    => file_uri_scheme($file['uri']),
        'media_uri' => $file['uri'],
      );

      slick_extract_image_data($settings, $media, $slide, $file);
      $slide['settings']['type'] = isset($file['type']) ? $file['type'] : 'image';

      // Image with picture, lazyLoad, and lightbox supports.
      $slide['slide'] = slick_get_image($settings, $media, $key, $slide['settings']);

      if ($settings['nav']) {
        // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
        $thumb['slide'] = slick_get_thumbnail($settings['thumbnail_style'], $media);
        if (!empty($settings['thumbnail_caption']) && $caption = $wrapper->{$settings['thumbnail_caption']}->value()) {
          $thumb['caption']['data']['#markup'] = isset($caption['safe_value']) ? $caption['safe_value'] : filter_xss($caption);
        }
        $build['thumb'][$key] = $thumb;
      }
    }

    // Main slide overlays.
    if ($settings['current_display'] == 'main') {
      // Nested slicks, or image/video/audio overlays applies to main display.
      // EMW fails to check empty for disabled fields, so use the fields.
      if (!empty($settings['slide_overlay']) && isset($fields[$settings['slide_overlay']])) {
        $slides = $wrapper->{$settings['slide_overlay']}->value();
        $renderables = $fields[$settings['slide_overlay']];
        $options = array(
          'id'          => $id,
          'key'         => $key,
          'nested_name' => $nested_name,
          'nested_set'  => $nested_set,
          'view_mode'   => $view_mode,
          'fields'      => $fields,
          'langcode'    => $langcode,
        );

        if (!in_array($renderables['#formatter'], array('image', 'file_rendered')) && !isset($slides[0]->sid)) {
          // Just display un-managed overlays with unknown formatters.
          $elements = $renderables;
        }
        else {
          // Otherwise do the hard work.
          $elements = _slick_build_fc_overlay($settings, $wrapper, $options);
        }
        $slide['caption']['overlay'] = $elements;
      }

      slick_get_caption($settings, $entity_type, $fc, $view_mode, $slide);

      // Layout field as opposed to the builtin layout options.
      if (!empty($settings['layout'])) {
        if (strpos($settings['layout'], 'field_') !== FALSE && $slide_layout = $wrapper->{$settings['layout']}->value()) {
          $slide['settings']['layout'] = strip_tags($slide_layout);
        }
      }

      // Color can be plain text, or color_field.module.
      // @todo CSS background lazyload when Blazy landed instead.
      if (!empty($settings['color_field']) && $colors = $wrapper->{$settings['color_field']}->value()) {
        $color = is_string($colors) ? $colors : (!empty($colors['rgb']) ? $colors['rgb'] : '');
        if (!empty($color)) {
          // @see http://tympanus.net/Development/CreativeLinkEffects/
          $light = slick_color_brightness($color, 42);
          $hover = slick_color_brightness($color, 62);

          $css[] = "#{$slick_id} .slide--{$key} {color: {$color}}";
          $css[] = "#{$slick_id} .slide--{$key} .slide__title {color: {$color}}";
          $css[] = "#{$slick_id} .slide--{$key} .slide__link a::before, #{$slick_id} .slide--{$key} .slide__link a::after {background-color: {$color}}";

          if ($color != '#fff' || $color != '#ffffff') {
            $css[] = "#{$slick_id} .slide--{$key} .slide__link a {color: {$color}}";
            $css[] = "#{$slick_id} .slide--{$key} .slide__link a:hover {color: {$hover}}";
            $css[] = "#{$slick_id} .slide--{$key} .slide__caption {color: {$light}}";
          }
        }
      }
    }

    $build['items'][$key] = $slide;
    unset($slide, $thumb);
  }
  unset($nested_set);
  if (isset($css)) {
    $settings['inline_css'] = $css;
    unset($css);
  }
  return $build;
}

/**
 * Build the managed overlays: image, video, audio or nested slicks.
 */
function _slick_build_fc_overlay(&$settings, $wrapper, $options = array()) {
  $elements     = array();
  $key          = $options['key'];
  $items        = $wrapper->{$settings['slide_overlay']}->value();
  $renderables  = $options['fields'][$settings['slide_overlay']];
  $entity_id    = $settings['entity_id'];
  $entity_type  = $renderables['#entity_type'];
  $bundle       = $renderables['#bundle'];
  $field_name   = $renderables['#field_name'];
  $nested_style = !empty($settings['nested_style']) ? $settings['nested_style'] : $settings['image_style'];
  $css_id       = slick_html_id("slick-{$entity_type}-{$entity_id}-{$bundle}-{$field_name}");

  $settings['picture_ready'] = FALSE;

  $overlays = array();

  // @todo refine more to $slide['settings'] and match Media/Image formatters.
  $overset = array(
    'box_style'       => $settings['colorbox_style'],
    'current_display' => 'overlay',
    'has_pattern'     => $settings['has_pattern'],
    'image_style'     => $nested_style,
    'lazy'            => $settings['lazy'],
    'media_switch'    => $settings['media_switch'],
    'ratio'           => empty($settings['aspect_ratio']) ? FALSE : $settings['aspect_ratio'],
    'view_mode'       => $options['view_mode'],
    'iframe_lazy'     => !empty($settings['iframe_lazy']),
  );

  switch ($renderables['#field_type']) {
    case 'image':
    case 'file':
      foreach ($items as $i => $item) {
        $slide = $media = array();
        slick_extract_image_data($settings, $media, $slide, $item);

        if ($renderables['#field_type'] == 'file') {
          $file = (object) $item;
          $media['scheme'] = file_uri_scheme($file->uri);
          $media['media_uri'] = $file->uri;
          slick_get_media_url($overset, $media, $file);

          // Get audio/video thumbnail uri.
          $media_image_uri = slick_get_media_thumbnail($overset, $media);
          if ($media_image_uri) {
            $media['uri'] = $media_image_uri;
          }
        }
        $overlays[$i] = $media + $overset;
      }
      break;

    case 'atom_reference':
      foreach ($items as $i => $item) {
        $sid             = $item->sid;
        $atom            = scald_atom_load($sid);
        $base_id         = $atom->base_id;
        $langcode        = isset($atom->language) ? $atom->language : $options['langcode'];
        $media['scheme'] = $atom->provider;

        $providers = array(
          'scald_youtube',
          'scald_vimeo',
          'scald_dailymotion',
          'video',
        );
        if (in_array($atom->provider, $providers)) {
          $overset['media_id'] = $base_id;
          if (strpos($atom->provider, 'scald_') !== FALSE) {
            $media['scheme'] = substr($atom->provider, -6);
          }
        }

        $media['embed_url'] = '';
        switch ($atom->provider) {
          case 'scald_youtube':
            $media['embed_url'] = '//www.youtube.com/embed/' . $base_id;
            break;

          case 'scald_vimeo':
            $media['embed_url'] = '//player.vimeo.com/video/' . $base_id;
            break;

          case 'scald_dailymotion':
            $media['embed_url'] = '//www.dailymotion.com/embed/video/' . $base_id;
            break;

          case 'video':
            $media['embed_url'] = $atom->file_source;
            break;

          default:
            break;
        }

        // Only care for the first item now.
        $properties = array('fid', 'height', 'width', 'alt', 'title', 'type');
        foreach ($properties as $data) {
          $media[$data] = isset($atom->scald_thumbnail[$langcode][0][$data]) ? $atom->scald_thumbnail[$langcode][0][$data] : NULL;
        }

        $media['uri']   = $atom->file_source;
        $media['type']  = $atom->type == 'slick' ? 'image' : $atom->type;
        $media['title'] = $atom->title;
        $overset['sid'] = $sid;

        $overlays[$i] = $media + $overset;
      }
      break;

    default:
      break;
  }

  $slides = $overlays;

  // Build the overlays: single static image/audio/video, or nested slicks.
  // Managed overlays with known formatters are nested slicks, or static.
  if ($slides) {
    $nested_settings = $overset + array(
      'count'          => count($slides),
      'id'             => $css_id,
      'lazy'           => $settings['lazy'],
      'lightbox_ready' => $settings['media_switch'] == 'colorbox-switch',
      'optionset'      => $options['nested_name'],
      'picture'        => FALSE,
      'picture_ready'  => FALSE,
      'attributes'     => array('class' => array('slick--nested')),
    );

    // Process individual slick overlay.
    $element = array();
    foreach ($slides as $idx => $media) {
      $nested_settings = array_merge($nested_settings, $media);
      if (empty($settings['nested_slick'])) {
        $idx = 0;
      }

      // Image with picture, lazyLoad, and lightbox supports.
      $slide = slick_get_image($nested_settings, $media, $idx, $media);
      $element[$idx]['slide'] = $slide;
      unset($media, $slide);
    }

    // Build the nested slicks.
    if (!empty($settings['nested_slick'])) {
      $attach['attach_media'] = $settings['media_switch'] == 'iframe-switch';
      $elements = slick_build($element, array(), $nested_settings, $attach, $css_id, $options['nested_set']);
    }
    else {
      // Otherwise cares for a single overlay item to not break small layout.
      $elements = $element[0];
    }
  }
  return $elements;
}