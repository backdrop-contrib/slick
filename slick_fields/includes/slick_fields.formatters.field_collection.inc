<?php
/**
 * @file
 * Fields formatter for the Slick and Field collection integration.
 */

/**
 * Formats Field collection data.
 */
function _slick_fields_format_field_collection(&$settings, $config, $thumb_nav = FALSE) {
  $view_mode        = $settings['view_mode'] ? $settings['view_mode'] : 'full';
  $image_style      = $settings['image_style'];
  $slick_id         = $settings['attributes']['id'];
  $optionset_name   = $settings['nested_optionset'] ? $settings['nested_optionset'] : 'default';
  $nested_optionset = empty($settings['nested_slick']) ? NULL : slick_optionset_load($optionset_name);
  $config['data']   = array_merge($config['data'], array('height', 'width'));

  $build = $css = array();
  foreach ($config['items'] as $key => $item) {
    $slide = array(
      'item'    => $item,
      'caption'  => array(),
      'settings' => array(
        'type'         => $settings['type'],
        'layout'       => $settings['slide_layout'],
        'media_switch' => $settings['media_switch'],
      ),
    );

    // Prepares variables.
    $fc        = field_collection_field_get_entity($item);
    $wrapper   = entity_metadata_wrapper('field_collection_item', $fc);
    $fc_view   = $fc->view($view_mode);
    $id        = array_keys($fc_view['field_collection_item']);
    $id        = reset($id);
    $fc_fields = $fc_view['field_collection_item'][$id];

    // Main background image can be core Image, or File entity.
    if ($settings['slide_image'] && $slide_bg = $wrapper->$settings['slide_image']->value()) {
      $file       = isset($slide_bg['fid']) ? $slide_bg : (isset($slide_bg[0]) ? $slide_bg[0] : NULL);
      $field_type = $slide['settings']['type'] = isset($file['type']) ? $file['type'] : 'image';

      // It is un-reasonable to make large image as colorbox, so disable it.
      $settings['lightbox_ready'] = FALSE;
      $settings['picture_ready'] = TRUE;

      switch ($field_type) {
        case 'file':
        case 'image':
          if ($file) {
            foreach ($config['data'] as $data) {
              $image[$data] = isset($file[$data]) ? $file[$data] : '';
            }
          }
          break;

        default:
          break;
      }

      // Provides thumbnail pager with the same field instance if so configured.
      $thumbnail = '';
      if ($thumbnail_style = $settings['thumbnail_style']) {
        $image['path']       = $image['uri'];
        $image['style_name'] = $thumbnail_style;
        $thumbnail_variables = $image;

        // Provides hidden static thumbnail revealed on dots hover.
        $image['attributes']['data-thumb'] = image_style_url($thumbnail_style, $image['uri']);
        $thumbnail = theme('image_style', $thumbnail_variables);
      }

      if (!$thumb_nav) {
        $slide['settings']['height']       = $image['height'];
        $slide['settings']['width']        = $image['width'];
        $slide['settings']['image_style']  = $image_style;
        $slide['settings']['current_item'] = 'main';
        $slide['settings']['has_pattern']  = $settings['has_pattern'];

        // Image with picture and colorbox supports.
        $slide['slide'] = slick_get_image($settings, $slide['settings'], $image, $key);
      }
      else {
        $slide['slide'] = $thumbnail;
        $slide['settings']['type'] = 'image';

        // Thumbnail captions.
        if ($settings['thumbnail_caption'] && $caption = $wrapper->$settings['thumbnail_caption']->value()) {
          $slide['caption']['data']['#markup'] = isset($caption['safe_value']) ? $caption['safe_value'] : filter_xss($caption['value']);
          $slide['settings']['type'] = empty($thumbnail) ? 'text' : 'image';
        }
      }

      // Fullscreen skin needs inline CSS if so configured.
      if ($config['inline_css']) {
        $slide['slide'] = array();
        $image_url = $image_style ? image_style_url($image_style, $image['uri']) : file_create_url($image['uri']);
        $css[] = "#{$slick_id} .slide--{$key} {background-image: url('{$image_url}')}";
      }
    }

    if (!$thumb_nav) {
      // Nested slicks, or image/video/audio overlays applies to main display.
      // EMW fails to check empty for disabled fields, so use the fields.
      if ($settings['slide_overlay'] && isset($fc_fields[$settings['slide_overlay']])) {
        $options = array(
          'data'             => $config['data'],
          'id'               => $id,
          'key'              => $key,
          'optionset_name'   => $optionset_name,
          'nested_optionset' => $nested_optionset,
          'view_mode'        => $view_mode,
          'fc_fields'        => $fc_fields,
          'langcode'         => $config['langcode'],
          'config'           => $config,
        );
        $slides      = $wrapper->$settings['slide_overlay']->value();
        $renderables = $fc_fields[$settings['slide_overlay']];
        $formatter   = $renderables['#formatter'];

        // Just display un-managed overlays with unknown formatters.
        if (!in_array($formatter, array('image', 'file_rendered')) && !isset($slides[0]->sid)) {
          $overlay_elements = $renderables;
        }
        // Otherwise do the hard-work.
        elseif ($overlays = _slick_fields_fc_build_overlay($settings, $wrapper, $options)) {
          $overlay_elements = $overlays;
        }
        $slide['caption']['overlay'] = $overlay_elements;
      }

      // Title, if so configured, can be plain text, or link field.
      if ($settings['slide_title'] && $slide_title = $wrapper->$settings['slide_title']->value()) {
        $title = '';
        if (isset($slide_title['safe_value'])) {
          $title = $slide_title['safe_value'];
        }
        elseif (isset($slide_title['url'])) {
          $title = $slide_title['title'];
          if ($url = $slide_title['url']) {
            $attributes = $slide_title['attributes'] ? array('attributes' => $slide_title['attributes']) : array();
            $title = l($title, $url, $attributes);
          }
        }
        if ($title) {
          $slide['caption']['title']['#markup'] = $title;
        }
      }

      // Other caption fields, if so configured.
      if ($slide_caption = $settings['slide_caption']) {
        $caption_items = array();
        foreach ($slide_caption as $i => $caption_field) {
          if (isset($fc_fields[$caption_field])) {
            $caption_items[$i] = $fc_fields[$caption_field];
          }
        }

        if ($caption_items) {
          $slide['caption']['data'] = $caption_items;
        }
      }

      // Link field if so configured, it is a renderable array.
      if ($settings['slide_link'] && isset($fc_fields[$settings['slide_link']])) {
        $slide['caption']['link'] = $fc_fields[$settings['slide_link']];
      }

      // Layout field as opposed from the builtin layout options.
      if (strpos($settings['slide_layout'], 'field_') !== FALSE && $layout = $wrapper->$settings['slide_layout']->value()) {
        $slide['settings']['layout'] = check_plain($layout);
      }

      // Color can be plain text, or color_field.module.
      if ($settings['color_field'] && $colors = $wrapper->$settings['color_field']->value()) {
        $color = is_string($colors) ? $colors : (!empty($colors['rgb']) ? $colors['rgb'] : '');
        if (!empty($color)) {
          $light = slick_color_brightness($color, 42);
          $hover = slick_color_brightness($color, 62);

          $css[] = "#{$slick_id} .slide--{$key} {color: {$color}}";
          $css[] = "#{$slick_id} .slide--{$key} .slide__title {color: {$color}}";
          // @see http://tympanus.net/Development/CreativeLinkEffects/
          $css[] = "#{$slick_id} .slide--{$key} .slide__link a::before, #{$slick_id} .slide--{$key} .slide__link a::after {background-color: {$color}}";

          if ($color != '#fff' || $color != '#ffffff') {
            $css[] = "#{$slick_id} .slide--{$key} .slide__link a {color: {$color}}";
            $css[] = "#{$slick_id} .slide--{$key} .slide__link a:hover {color: {$hover}}";
            $css[] = "#{$slick_id} .slide--{$key} .slide__caption {color: {$light}}";
          }
        }
      }
    }

    $build[$key] = $slide;
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }

  return $build;
}

/**
 * Build the managed overlays: image, video, audio or nested slicks.
 */
function _slick_fields_fc_build_overlay(&$settings, $wrapper, $options = array()) {
  $elements                = array();
  $key                     = $options['key'];
  $image_style             = $settings['image_style'];
  $slides                  = $wrapper->$settings['slide_overlay']->value();
  $renderables             = $options['fc_fields'][$settings['slide_overlay']];
  $entity_id               = $settings['entity_id'];
  $field_type              = $renderables['#field_type'];
  $entity_type             = $renderables['#entity_type'];
  $bundle                  = $renderables['#bundle'];
  $field_name              = $renderables['#field_name'];
  $formatter               = $renderables['#formatter'];
  $overlay_style           = $settings['nested_style'] ? $settings['nested_style'] : $image_style;
  $is_ratio                = $settings['media_switch'] && $settings['aspect_ratio'];
  $settings['has_overlay'] = $settings['lightbox_ready'] = TRUE;
  $settings['is_media']    = $settings['picture_ready'] = FALSE;
  $optionset_name          = $options['optionset_name'];
  $nested_optionset        = $options['nested_optionset'];
  $slick_id                = &drupal_static('slick_id', 0);
  $id                      = drupal_clean_css_identifier("slick-{$entity_type}-{$entity_id}-{$bundle}-{$field_name}-" . ++$slick_id);

  $overlays = $overlay = array();
  switch ($field_type) {
    case 'image':
    case 'file':
      if (in_array($formatter, array('image', 'file_rendered'))) {
        foreach ($slides as $f => $slide) {
          foreach ($options['data'] as $data) {
            $overlay[$data] = isset($slide[$data]) ? $slide[$data] : NULL;
            // No dimensions for media file, except image.
            if (isset($slide['metadata'][$data])) {
              $overlay[$data] = $slide['metadata'][$data];
            }
          }

          // Without Media module, image has no type, so set it.
          $overlay['type']        = empty($overlay['type']) ? 'image' : $overlay['type'];
          $overlay['image_style'] = $overlay_style;

          $overset = array(
            'image_style'  => $overlay_style,
            'media_switch' => $settings['media_switch'],
            'ratio'        => $is_ratio ? $settings['aspect_ratio'] : FALSE,
            'type'         => $overlay['type'],
          );

          if ($field_type == 'file') {
            $file = (object) $slide;
            $uri = $overlay['uri'];
            $scheme = file_uri_scheme($uri);

            // If a file has invalid type, allow file_view_file() to work.
            if (!file_type_is_enabled($file->type)) {
              $file->type = file_get_type($file);
            }

            $overset += array(
              'has_media' => TRUE,
              'media_uri' => $uri,
              'scheme'    => $scheme,
            );

            // Get audio/video thumbnail uri.
            $media_image_uri = slick_get_media_thumbnail($overset, $uri);
            if ($media_image_uri) {
              // Make the URI reference thumbnail image. Video to media_uri.
              $overlay['uri'] = $media_image_uri;
              $media_id       = drupal_html_id('media-' . $scheme . '-' . $overset['media_id']);
              $overset['content_attributes']['id'] = $overset['api_id_attribute'] = $media_id;
            }

            // $iframe is a flag to check that we have an iframe to render.
            // $media_url is expected by lightboxes to display in a lightbox.
            // If has Media fields by media.module with iframe source.
            $iframe = file_view_file($file, $options['view_mode']);
            if ($iframe && isset($iframe['#uri'])) {
              $overset['is_media'] = TRUE;
              $rendered_iframe     = render($iframe);
              $media_url           = slick_get_media_url($rendered_iframe);
              if ($media_url) {
                $overlay['url'] = $media_url;
              }
            }
          }
          $overlay += $overset;
          $overlays[$f] = $overlay;
        }
      }
      break;

    case 'atom_reference':
      if (isset($slides[0]->sid)) {
        foreach ($slides as $a => $slide) {
          $sid                 = $slide->sid;
          $atom                = scald_atom_load($sid);
          $base_id             = $atom->base_id;
          $langcode            = isset($atom->language) ? $atom->language : $options['langcode'];
          $overset['scheme']   = $atom->provider;
          $overset['media_id'] = $base_id;

          $providers = array(
            'scald_youtube',
            'scald_vimeo',
            'scald_dailymotion',
            'video',
          );
          if (in_array($atom->provider, $providers)) {
            // We do have iframe, set it to TRUE.
            $overset['has_media'] = $overset['is_media'] = $iframe = TRUE;
            $overset['media_switch'] = $settings['media_switch'];
            if (strpos($atom->provider, 'scald_') !== FALSE) {
              $overset['scheme'] = substr($atom->provider, -6);
            }
          }

          $overlay['url'] = '';
          switch ($atom->provider) {
            case 'scald_youtube':
              $overlay['url'] = '//www.youtube.com/embed/' . $base_id;
              break;

            case 'scald_vimeo':
              $overlay['url'] = '//player.vimeo.com/video/' . $base_id;
              break;

            case 'scald_dailymotion':
              $overlay['url'] = '//www.dailymotion.com/embed/video/' . $base_id;
              break;

            case 'video':
              $overlay['url'] = $atom->file_source;
              break;

            default:
              break;
          }

          $overlay['uri'] = $atom->file_source;

          // Only care for the first item now.
          foreach ($options['data'] as $data) {
            $overlay[$data] = isset($atom->scald_thumbnail[$langcode][0][$data]) ? $atom->scald_thumbnail[$langcode][0][$data] : NULL;
          }

          $overlay['image_style'] = $overlay_style;
          $overlay['type']        = $atom->type == 'slick' ? 'image' : $atom->type;
          $overset['filename']    = check_plain($atom->title);
          $overset['sid']         = $sid;
          $overset['ratio']       = $is_ratio ? $settings['aspect_ratio'] : FALSE;

          $overlay += $overset;
          $overlays[$a] = $overlay;
        }
      }
      break;

    default:
      break;
  }

  $overlays = $overlays;

  // Build the overlays: single static image/audio/video, or nested slicks.
  // Managed overlays with known formatters are nested slicks, or static.
  if ($overlays) {
    $overlay_settings = array(
      'count'           => count($overlays),
      'lightbox_ready'  => $settings['media_switch'] == 'colorbox-switch',
      'colorbox_style'  => $settings['colorbox_style'],
      'picture_ready'   => FALSE,
      'picture'         => FALSE,
      'lazy'            => $settings['lazy'],
      'has_pattern'     => $settings['has_pattern'],
      'media_switch'    => $settings['media_switch'],
      'lightbox_id'     => $id,
      'current_display' => 'overlay',
      'optionset'       => $optionset_name,
      'image_style'     => $overlay_style,
      'ratio'           => $is_ratio ? $settings['aspect_ratio'] : FALSE,
      'attributes' => array(
        'id'    => $id,
        'class' => array('slick--nested'),
      ),
    );

    // Process individual slick overlay.
    $element = array();
    foreach ($overlays as $idx => $image) {
      $media_url     = isset($image['url']) && $image['url'] ? $image['url'] : '';
      $main_settings = array_merge($overlay_settings, $image);

      if (!$settings['nested_slick']) {
        $idx = 0;
      }
      $overlay_item[$idx] = slick_get_image($main_settings, $image, $image, $idx, $media_url);

      // Build the media.
      $element[$idx]['slide'] = array(
        '#theme'    => 'slick_media',
        '#item'     => $overlay_item[$idx],
        '#settings' => $image,
        '#display'  => 'overlay',
      );
    }

    // Build the nested slicks.
    if ($settings['nested_slick']) {
      $attach['attach_media'] = $settings['media_switch'] == 'iframe-switch';
      $elements = slick_build($element, array(), $overlay_settings, $attach, NULL, $nested_optionset);
    }
    else {
      // Otherwise cares for a single overlay item to not break small layout.
      $elements = $element[0];
    }
  }

  return $elements;
}
