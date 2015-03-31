<?php
/**
 * @file
 * Fields formatter for the Slick and Field collection integration.
 */

/**
 * Format Field collection data.
 */
function _slick_fields_format_field_collection(&$settings, $items, $field, $view_mode, $langcode, $thumb_nav = FALSE) {
  module_load_include('inc', 'slick', 'includes/slick.extras');

  $build = array();
  $css = array();
  $field_type = $field['type'];
  $image_style = $settings['image_style'];
  $slick_id = $settings['attributes']['id'];

  foreach ($items as $key => $item) {
    $slide = array(
      'item' => $item,
      'slide' => array(),
      'caption' => array(),
    );

    if ($fc = field_collection_field_get_entity($slide['item'])) {
      $fc_view = $fc->view($view_mode);
      $fc = (array) $fc;

      // Get field collection ID.
      $id = array_keys($fc_view['field_collection_item']);
      $id = reset($id);
      $fc_fields = $fc_view['field_collection_item'][$id];

      // Manually insert type for theme to get the context.
      $slide['item']['type'] = 'field_collection';
      $settings['type'] = $slide['item']['type'];

      // Customize display to tame Field collection.
      // Main background image.
      if ($settings['slide_image'] && isset($fc_fields[$settings['slide_image']])) {
        $slide_image = $fc_fields[$settings['slide_image']];
        $field_type = $slide_image['#field_type'];

        // It is un-reasonable to make large image as colorbox, so disable it.
        $settings['lightbox_ready'] = FALSE;
        $settings['picture_ready'] = TRUE;
        switch ($field_type) {
          case 'file':
            // Large background video is not supported, any taker?
            $file            = $slide_image[0]['file'];
            $fallback        = $slide_image['#items'][0];
            $image_style     = isset($file['#style_name']) ? $file['#style_name'] : (isset($file['#image_style']) ? $file['#image_style'] : $image_style);
            $image['uri']    = isset($file['#path']) && $file['#path'] ? $file['#path'] : $fallback['uri'];
            $image['width']  = isset($file['#width']) ? $file['#width'] : NULL;
            $image['height'] = isset($file['#height']) ? $file['#height'] : NULL;
            $image['alt']    = isset($file['#alt']) ? $file['#alt'] : $fallback['alt'];
            $image['title']  = isset($file['#title']) ? $file['#title'] : $fallback['title'];

            break;

          case 'image':
            $file            = $slide_image[0]['#item'];
            $image_style     = isset($file['#image_style']) ? $file['#image_style'] : $image_style;
            $image['uri']    = $file['uri'];
            $image['width']  = $file['width'];
            $image['height'] = $file['height'];
            $image['alt']    = $file['alt'];
            $image['title']  = $file['title'];

            break;

          default:
            break;
        }

        // Provide thumbnail pagers if so configured.
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
          $settings['type']         = $field_type;
          $settings['media_height'] = $image['height'];
          $settings['media_width']  = $image['width'];
          $settings['image_style']  = $image_style;
          $settings['current_item'] = 'main';
          // Image with picture and colorbox supports.
          $slide['slide'] = _slick_fields_get_picture($settings, $image, $key);
        }
        else {
          $slide['slide'] = $thumbnail;
          $settings['type'] = 'image';

          // Thumbnail captions.
          if ($thumbnail_caption = $settings['thumbnail_caption']) {
            if (isset($fc[$thumbnail_caption][$langcode][0]['safe_value']) && $fc[$thumbnail_caption][$langcode][0]['safe_value']) {
              $slide['caption']['data']['#markup'] = $fc[$thumbnail_caption][$langcode][0]['safe_value'];
            }
          }
        }

        // Use slick_inline_css_output_info_alter() to modify the outputs.
        $inline_css_skins = slick_inline_css_skins();
        if (in_array($settings['skin'], $inline_css_skins)) {
          $slide['slide'] = array();
          $fullscreen_image_url = image_style_url($settings['image_style'], $image['uri']);
          $css[] = "#{$slick_id} .slide--{$key} {background-image: url('{$fullscreen_image_url}')}";
        }
      }

      // Applies to main display only, excluding thumbnail navigation.
      if (!$thumb_nav) {
        // Overlay image/video.
        if ($settings['slide_overlay'] && isset($fc_fields[$settings['slide_overlay']])) {
          $slide_overlay = $fc_fields[$settings['slide_overlay']];

          $settings['has_overlay']    = TRUE;
          $settings['is_media']       = FALSE;
          $settings['lightbox_ready'] = TRUE;
          $settings['picture_ready']  = FALSE;
          $optionset = $settings['nested_optionset'];
          $optionset = slick_optionset_load($optionset);

          // Makes consistent image styles across slicks within a slick.
          if ($settings['nested_style']) {
            $overlay_style = $settings['nested_style'];
          }

          $overlay = array(
            'url' => '',
            'type' => '',
          );
          $overlays = $overlay_items = $oversets = array();
          $overlay_settings = array();

          $field_type = $slide_overlay['#field_type'];
          $formatter = $slide_overlay['#formatter'];

          // Media is a flag to determine that we have iframe to render.
          // Media URL is expected by colorbox if so configured.
          $media = $media_url = '';

          switch ($field_type) {
            case 'image':
              if (in_array($formatter, array('image', 'file_rendered'))) {
                $overset = array();
                foreach ($slide_overlay['#items'] as $i => $sub_overlay) {
                  $rendered          = $slide_overlay[$i];
                  $overlay['uri']    = $sub_overlay['uri'];
                  $overlay['width']  = $sub_overlay['width'];
                  $overlay['height'] = $sub_overlay['height'];
                  $overlay['alt']    = $sub_overlay['alt'];
                  $overlay['title']  = $sub_overlay['title'];

                  if (!isset($overlay_style)) {
                    $overlay_style = isset($rendered['#image_style']) ? $rendered['#image_style'] : $image_style;
                  }

                  $overset['type'] = $overlay['type'] = isset($sub_overlay['type']) ? $sub_overlay['type'] : 'image';
                  $overset['image_style'] = $overlay_style;

                  $overlays[$i] = $overlay;
                  $oversets[$i] = $overset;
                }

                $overlay_items[$key] = $overlays;
                $overlay_settings[$key] = $oversets;
              }

              break;

            case 'file':
              if (in_array($formatter, array('image', 'file_rendered'))) {
                $overset = array();
                foreach ($slide_overlay['#items'] as $f => $sub_overlay) {
                  $file = (object) $sub_overlay;

                  $uri = $file->uri;
                  $file_style = $slide_overlay[$f]['file'];

                  // Make sure view mode to enable image for the overlay, and
                  // they should come first before actual video.
                  if (!isset($overlay_style)) {
                    // Video uses image_style: #style_name.
                    // Image uses image_formatter: #image_style.
                    $overlay_style = isset($file_style['#style_name']) ? $file_style['#style_name'] : $image_style;
                  }

                  $overlay['uri']    = $uri;
                  $overlay['width']  = isset($file_style['#width']) ? $file_style['#width'] : NULL;
                  $overlay['height'] = isset($file_style['#height']) ? $file_style['#height'] : NULL;
                  $overlay['alt']    = $file->alt;
                  $overlay['title']  = $file->title;

                  // If a file has invalid type, allow file_view_file() to work.
                  if (!file_type_is_enabled($file->type)) {
                    $file->type = file_get_type($file);
                  }

                  $overset['type']         = $overlay['type'] = $file->type;
                  $overset['scheme']       = file_uri_scheme($uri);
                  $overset['has_media']    = TRUE;
                  $overset['media_uri']    = $uri;
                  $overset['media_fid']    = $file->fid;
                  $overset['media_height'] = $overlay['height'];
                  $overset['media_width']  = $overlay['width'];
                  $overset['image_style']  = $overlay_style;
                  $overset['filename']     = $file->filename;

                  // Get audio/video thumbnail uri.
                  if ($media_image_uri = _slick_fields_get_media_thumbnail($overset, $uri, $overlay_style)) {
                    $overlay['uri'] = $media_image_uri;
                  }

                  // If has Media fields by media.module for sure.
                  $media = file_view_file($file, $view_mode);

                  if ($media && isset($media['#uri'])) {
                    $overset['is_media'] = TRUE;
                    if (isset($media['#options'])) {
                      $overset['media_height'] = $media['#options']['height'] ? $media['#options']['height'] : 640;
                      $overset['media_width']  = $media['#options']['width'] ? $media['#options']['width'] : 360;
                    }

                    $rendered_media = render($media);
                    $media_url = slick_get_media_url($rendered_media);
                    if ($media_url) {
                      $overset['url'] = $overlay['url'] = $media_url;
                    }
                  }

                  if ($file->type == 'image') {
                    unset($overlay['url']);
                  }

                  $overlays[$f] = $overlay;
                  $oversets[$f] = $overset;
                }

                $overlay_items[$key] = $overlays;
                $overlay_settings[$key] = $oversets;
              }
              break;

            case 'atom_reference':
              $overset = array();
              foreach ($slide_overlay['#items'] as $a => $atom_item) {
                $sid = $atom_item['sid'];
                $atom = scald_atom_load($sid);
                $config = scald_context_config_load($formatter);
                $base_id = $atom->base_id;
                $langcode = isset($atom->language) ? $atom->language : $langcode;

                if (!isset($overlay_style) && ($transcoder = $config->transcoder[$atom->type]['*']) !== NULL) {
                  if ($transcoder != 'passthrough') {
                    if (preg_match('/^style-(.*)$/', $transcoder, $match)) {
                      $overlay_style = $match[1];
                    }
                  }
                }

                $overset['scheme'] = $atom->provider;
                $overset['media_id'] = $base_id;

                $providers = array(
                  'scald_youtube',
                  'scald_vimeo',
                  'scald_dailymotion',
                  'video',
                );
                if (in_array($atom->provider, $providers)) {
                  $overset['has_media'] = TRUE;
                  $overset['is_media'] = TRUE;
                  // We do have iframe, set it to TRUE.
                  $media = TRUE;
                  if (strpos($atom->provider, 'scald_') !== FALSE) {
                    $overset['scheme'] = substr($atom->provider, -6);
                  }
                }

                $overset['url'] = '';
                switch ($atom->provider) {
                  case 'scald_youtube':
                    $overset['url'] = '//www.youtube.com/embed/' . $base_id;
                    break;

                  case 'scald_vimeo':
                    $overset['url'] = '//player.vimeo.com/video/' . $base_id;
                    break;

                  case 'scald_dailymotion':
                    $overset['url'] = '//www.dailymotion.com/embed/video/' . $base_id;
                    break;

                  case 'video':
                    $overset['url'] = $atom->file_source;
                    break;

                  default:
                    break;
                }

                $overlay['uri'] = $atom->file_source;
                $overlay['url'] = $overset['url'];

                // Alternate content.
                $overset['filename'] = check_plain($atom->title);
                $overset['media_fid'] = $sid;

                $overlay_height = '';
                $overlay_width = '';

                // Only care for the first item now.
                if (isset($atom->scald_thumbnail[$langcode][0]['height'])) {
                  $overlay_height = $atom->scald_thumbnail[$langcode][0]['height'];
                }
                elseif (isset($atom->data['video_height'])) {
                  $overlay_height = $atom->data['video_height'];
                }

                if (isset($atom->scald_thumbnail[$langcode][0]['width'])) {
                  $overlay_width = $atom->scald_thumbnail[$langcode][0]['width'];
                }
                elseif (isset($atom->data['video_width'])) {
                  $overlay_width = $atom->data['video_width'];
                }

                if (isset($atom->scald_thumbnail[$langcode][0]['alt'])) {
                  $overlay['alt'] = $atom->scald_thumbnail[$langcode][0]['alt'];
                }
                if (isset($atom->scald_thumbnail[$langcode][0]['title'])) {
                  $overlay['title'] = $atom->scald_thumbnail[$langcode][0]['title'];
                }

                $dimensions = array(
                  'width' => $overlay_width,
                  'height' => $overlay_height,
                );

                image_style_transform_dimensions($overlay_style, $dimensions);

                $overlay['height'] = $overset['media_height'] = $dimensions['height'];
                $overlay['width']  = $overset['media_width'] = $dimensions['width'];

                $overset['type'] = $overlay['type'] = $atom->type == 'slick' ? 'image' : $atom->type;
                $overset['image_style'] = $overlay_style;

                $overlays[$a] = $overlay;
                $oversets[$a] = $overset;
              }

              $overlay_items[$key] = $overlays;
              $overlay_settings[$key] = $oversets;

              break;

            default:
              break;
          }

          $overlay_elements = array();

          // Managed overlays for nested slicks with supported formatters.
          if ($overlay_items) {
            $id = '';
            $overlay_element = array();
            $overlay_count = count($overlay_items[$key]);
            foreach ($overlay_items[$key] as $idx => $sub_overlay) {
              $media_url = isset($sub_overlay['url']) && $sub_overlay['url'] ? $sub_overlay['url'] : '';
              // Image with picture supports, but disabled for small overlay,
              // since we don't provide UI to manage small pictures, and
              // picture is only reasonable for large images.
              $overlay_settings[$key][$idx]['count']          = $overlay_count;
              $overlay_settings[$key][$idx]['lightbox_ready'] = $settings['media_switch'] == 'colorbox-switch';
              $overlay_settings[$key][$idx]['colorbox_style'] = $settings['colorbox_style'];
              $overlay_settings[$key][$idx]['picture_ready']  = FALSE;
              $overlay_settings[$key][$idx]['media_switch']   = $settings['media_switch'];
              $overlay_settings[$key][$idx]['picture']        = FALSE;
              $overlay_settings[$key][$idx]['lazy']           = $settings['lazy'];
              $overlay_settings[$key][$idx]['attributes']     = $settings['attributes'];

              // Must be similar across all slide items within a particular
              // Slick for the colorbox gallery ID - GID to work.
              $id = $settings['attributes']['id'] . '-' . $key;
              $overlay_settings[$key][$idx]['lightbox_id'] = $id;
            }

            if ($settings['nested_slick']) {
              foreach ($overlay_items[$key] as $idx => $sub_overlay) {
                $overlay_item = _slick_fields_get_picture($overlay_settings[$key][$idx], $sub_overlay, $key, $media_url);

                // Must be unique based on each slick slider.
                $id = $id . '-' . $idx;

                // Pass it over to theme_slick_media().
                $overlay_element[$idx] = array(
                  '#theme' => 'slick_media',
                  '#item' => $overlay_item,
                  '#settings' => $overlay_settings[$key][$idx],
                  '#asnavfor' => 'overlay',
                );

                // If has Media fields, provide a flag to render iframe.
                if (isset($overlay_settings[$key][$idx]['is_media'])) {
                  $overlay_element[$idx]['#media'] = $media;
                }
              }

              $overlay_settings[$key]['current_display'] = 'overlay';
              $overlay_settings[$key]['optionset'] = $settings['nested_optionset'] ? $settings['nested_optionset'] : 'default';

              if (!isset($overlay_settings[$key]['skin'])) {
                $overlay_settings[$key]['skin'] = $optionset->skin;
              }

              // Build the nested slicks, slicks within a slick.
              if ($id) {
                $overlay_settings[$key]['attributes']['id'] = $id;
              }
              $attach['attach_media'] = TRUE;
              $overlay_elements[$key] = array(
                '#theme' => 'slick',
                '#items' => $overlay_element,
                '#settings' => $overlay_settings[$key],
                '#attached' => slick_attach($attach),
              );

              // Overrides common options to re-use an optionset.
              if ($settings['override']) {
                $overridables = array();
                // Respect empty values, meaning FALSE.
                foreach ($settings['overridables'] as $key => $override) {
                  $override = empty($override) ? FALSE : TRUE;
                  $overridables[$key] = $override;
                }
                $overlay_elements[$key]['#options'] = $overridables;
              }

              // Stop craziness.
              $settings['nested_slick'] = FALSE;
            }
            // Otherwise a single overlay item.
            else {
              $overlay_item = _slick_fields_get_picture($overlay_settings[$key][0], $overlay_items[$key][0], $key, $media_url);

              // Pass it over to theme_slick_media.
              $overlay_elements = array(
                '#theme' => 'slick_media',
                '#item' => $overlay_item,
                '#settings' => $overlay_settings[$key][0],
                '#asnavfor' => 'overlay',
              );

              // If has Media fields.
              if (isset($overlay_settings[$key][0]['is_media'])) {
                $overlay_elements['#media'] = TRUE;
              }
            }
          }
          // Un-managed overlays for nested slicks with unknown formatters.
          else {
            $overlay_elements = $slide_overlay;
          }

          // Build the overlay elements.
          if ($overlay_elements) {
            $slide['caption']['overlay'] = $overlay_elements;
          }
        }

        // Title, if so configured.
        if ($slide_title = $settings['slide_title']) {
          $title = '';
          // Plain text title.
          if (isset($fc[$slide_title][$langcode][0]['safe_value'])) {
            $title = $fc[$slide_title][$langcode][0]['safe_value'];
          }
          // Link field title.
          elseif (isset($fc[$slide_title][$langcode][0]['url'])) {
            $url = '';
            $title = $fc[$slide_title][$langcode][0]['title'];
            if ($url = $fc[$slide_title][$langcode][0]['url']) {
              // $attributes = $fc[$slide_title][$langcode][0]['attributes'];
              $title = l($title, $url);
            }
          }

          $slide['caption']['title'] = $title;
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

        // Link, if so configured.
        if ($settings['slide_link'] && isset($fc_fields[$settings['slide_link']])) {
          $slide['caption']['link'] = $fc_fields[$settings['slide_link']];
        }

        // Layout, if so configured.
        if ($settings['slide_layout'] && isset($fc[$settings['slide_layout']][$langcode][0]['value'])) {
          $layout = $fc[$settings['slide_layout']][$langcode][0]['value'];
          $slide['caption']['layout'] = check_plain($layout);
        }

        // Color.
        if ($settings['color_field'] && isset($fc_fields[$settings['color_field']])) {
          $color_field = $fc_fields[$settings['color_field']];
          $field_type = $color_field['#field_type'];
          $color = '';

          switch ($field_type) {
            case 'text':
              $color = $color_field['#items'][0]['safe_value'];
              break;

            case 'color_field_rgb':
              $color = strip_tags($color_field['#items'][0]['rgb']);
              break;
          }

          if ($color) {
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
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }

  return $build;
}
