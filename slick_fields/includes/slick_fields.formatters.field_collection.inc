<?php

/**
 * @file
 * Fields formatter for the Slick and Field collection integration.
 */

/**
 * Format field collection data.
 */
function _slick_fields_format_field_collection(&$settings, $items, $field, $view_mode, $langcode, $thumb_nav = FALSE) {
  module_load_include('inc', 'slick', 'includes/slick.extras');

  $build = array();
  $css = array();
  $field_type = $field['type'];
  $image_style = $settings['image_style'];
  $slick_id = $settings['attributes']['id'];
  $asnavfor = 'main';

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
      if ($settings['slide_image']) {
        if (isset($fc_fields[$settings['slide_image']])) {
          $slide_image = $fc_fields[$settings['slide_image']];
          $field_type = $slide_image['#field_type'];

          // It is un-reasonable to make large image as colorbox, so disable it.
          $settings['colorbox_ready'] = FALSE;
          $settings['picture_ready'] = TRUE;
          switch ($field_type) {
            case 'file':
              // Large background video is not supported, any taker?
              $file = $slide_image[0]['file'];
              $fallback = $slide_image['#items'][0];
              // @todo recheck versions, #image_style vs #style_name
              $image_style = isset($file['#style_name']) ? $file['#style_name'] : (isset($file['#image_style']) ? $file['#image_style'] : $image_style);
              $image['uri'] = isset($file['#path']) && $file['#path'] ? $file['#path'] : $fallback['uri'];
              $image['width'] = isset($file['#width']) ? $file['#width'] : NULL;
              $image['height'] = isset($file['#height']) ? $file['#height'] : NULL;
              $image['alt'] = isset($file['#alt']) ? $file['#alt'] : $fallback['alt'];
              $image['title'] = isset($file['#title']) ? $file['#title'] : $fallback['title'];

              break;

            case 'image':
              $file = $slide_image[0]['#item'];
              $image_style = isset($file['#image_style']) ? $file['#image_style'] : $image_style;
              $image['uri'] = $file['uri'];
              $image['width'] = $file['width'];
              $image['height'] = $file['height'];
              $image['alt'] = $file['alt'];
              $image['title'] = $file['title'];

              break;

            // @todo or drop scald atom_reference.
          }

          // Provide thumbnail pagers if so configured.
          // @todo use renderable array for D8, and convert slide to array.
          $thumbnail = '';
          $thumbnail_hover = '';

          if (isset($settings['thumbnail_style']) && $settings['thumbnail_style']) {
            $image['path'] = $image['uri'];
            $image['style_name'] = $settings['thumbnail_style'];

            $thumbnail = theme('image_style', $image);

            if ($settings['thumbnail_hover']) {
              $thumbnail_hover = '<div class="slide__thumbnail--placeholder"><div class="slide__thumbnail">' . $thumbnail . '</div>';
              $thumbnail_hover .= '<button type="button" data-role="none">' . ($key + 1) . '</button></div>';
            }
          }

          if (!$thumb_nav) {
            $settings['type'] = $field_type;
            $settings['media_height'] = $image['height'];
            $settings['media_width'] = $image['width'];
            $settings['image_style'] = $image_style;
            // Image with picture and colorbox supports.
            $slide['slide'] = _slick_fields_get_picture($settings, $image, $key);

            // Thumbnail static to copy over by JS.
            if ($settings['dots_main'] && $thumbnail_hover) {
              $slide['slide'] .= $thumbnail_hover;
            }
          }
          else {
            $slide['slide'] = $thumbnail;
            $settings['type'] = 'image';
            $asnavfor = 'thumbnail';

            // Thumbnail static to copy over by JS.
            if ($settings['dots_thumbnail'] && $thumbnail_hover) {
              $slide['slide'] .= $thumbnail_hover;
            }
          }

          // @todo map from picture, or refine for mobile.
          if (in_array($settings['skin'], array('fullscreen', 'parallax'))) {
            $slide['slide'] = array();
            $fullscreen_image_url = image_style_url($settings['image_style'], $image['uri']);
            $css[] = "#{$slick_id} .slide--{$key} {background-image: url('{$fullscreen_image_url}')}";
          }
        }
      }

      // Applies to main display only, excluding thumbnail navigation.
      if (!$thumb_nav) {
        // Overlay image/video.
        if ($settings['slide_overlay']) {
          if (isset($fc_fields[$settings['slide_overlay']])) {
            $settings['has_overlay'] = TRUE;
            $settings['is_media'] = FALSE;
            $settings['colorbox_ready'] = TRUE;
            $settings['picture_ready'] = FALSE;
            $slide_overlay = $fc_fields[$settings['slide_overlay']];
            $field_type = $slide_overlay['#field_type'];
            $formatter = $slide_overlay['#formatter'];

            // Media is a flag to determine that we have iframe to render.
            $media = '';
            // Media URL is expected by colorbox if so configured.
            $media_url = '';
            switch ($field_type) {
              case 'image':
                $file = $slide_overlay[0]['#item'];
                $overlay_style = isset($file['#image_style']) ? $file['#image_style'] : $image_style;
                $overlay['uri'] = $file['uri'];
                $overlay['width'] = $file['width'];
                $overlay['height'] = $file['height'];
                $overlay['alt'] = $file['alt'];
                $overlay['title'] = $file['title'];
                $settings['type'] = 'image';

                break;

              case 'file':
                $fid = $slide_overlay['#items'][0]['fid'];
                $file = $slide_overlay['#items'][0];
                $slide_type = $file['type'];
                $file_style = $slide_overlay[0]['file'];

                // Make sure view mode to enable image for the overlay, and they
                // should come first before actual video.
                $overlay_style = isset($file_style['#style_name']) ? $file_style['#style_name'] : $image_style;
                $overlay['uri'] = $file['uri'];
                $overlay['width'] = isset($file_style['#width']) ? $file_style['#width'] : NULL;
                $overlay['height'] = isset($file_style['#height']) ? $file_style['#height'] : NULL;
                $overlay['alt'] = $file['alt'];
                $overlay['title'] = $file['title'];

                $settings['media_height'] = $overlay['height'];
                $settings['media_width'] = $overlay['width'];

                // Make object for file_view_file().
                $file = (object) $file;
                $uri = $file->uri;
                // If a file has invalid type, allow file_view_file() to work.
                if (!file_type_is_enabled($file->type)) {
                  $file->type = file_get_type($file);
                }

                $media = file_view_file($file, $view_mode);
                $scheme = file_uri_scheme($uri);

                $settings['scheme'] = $scheme;
                $settings['type'] = $slide_type;
                $settings['has_media'] = TRUE;
                $settings['media_uri'] = $uri;
                $settings['media_fid'] = $file->fid;
                $settings['media_filename'] = $file->filename;

                // Get audio/video thumbnail uri.
                if ($media_image_uri = _slick_fields_get_media_thumbnail($settings, $uri, $overlay_style)) {
                  $overlay['uri'] = $media_image_uri;
                }

                // If has Media fields by media.module for sure.
                if ($media && isset($media['#uri'])) {
                  $wrapper = file_stream_wrapper_get_instance_by_uri($media['#uri']);
                  $settings['is_media'] = TRUE;
                  if (isset($media['#options'])) {
                    $settings['media_height'] = $media['#options']['height'] ? $media['#options']['height'] : 640;
                    $settings['media_width'] = $media['#options']['width'] ? $media['#options']['width'] : 360;
                  }

                  $rendered_media = render($media);
                  $safe = TRUE;
                  if ($settings['scheme'] == 'soundcloud') {
                    $safe = FALSE;
                    $soundcloud_url = $wrapper->interpolateUrl();
                    // Prevent fatal error for no internet connection.
                    if ($embed_data = $wrapper->getOEmbed($soundcloud_url)) {
                      $settings['soundcloud'] = TRUE;
                      $safe = TRUE;
                    }
                  }

                  if ($safe) {
                    $media_url = slick_get_media_url($rendered_media);
                    $settings['url'] = $media_url;
                  }
                }

                break;

              case 'atom_reference':
                $sid = $slide_overlay['#items'][0]['sid'];
                $atom = scald_atom_load($sid);
                $config = scald_context_config_load($formatter);
                $base_id = $atom->base_id;
                $langcode = isset($atom->language) ? $atom->language : $langcode;

                $overlay['uri'] = $atom->file_source;
                $overlay_style = $image_style;

                if ($transcoder = $config->transcoder[$atom->type]['*']) {
                  if ($transcoder != 'passthrough') {
                    if (preg_match('/^style-(.*)$/', $transcoder, $match)) {
                      $overlay_style = $match[1];
                    }
                  }
                }

                $settings['scheme'] = $atom->provider;
                $settings['media_id'] = $base_id;
                $providers = array(
                  'scald_youtube',
                  'scald_vimeo',
                  'scald_dailymotion',
                  'video',
                );
                if (in_array($atom->provider, $providers)) {
                  $settings['has_media'] = TRUE;
                  $settings['is_media'] = TRUE;
                  // We do have iframe, set it to TRUE.
                  $media = TRUE;
                  if (strpos($atom->provider, 'scald_') !== FALSE) {
                    $settings['scheme'] = substr($atom->provider, -6);
                  }
                }

                switch ($atom->provider) {
                  case 'scald_youtube':
                    $settings['url'] = '//www.youtube.com/embed/' . $base_id;
                    break;

                  case 'scald_vimeo':
                    $settings['url'] = '//player.vimeo.com/video/' . $base_id;
                    break;

                  case 'scald_dailymotion':
                    $settings['url'] = '//www.dailymotion.com/embed/video/' . $base_id;
                    break;

                  case 'video':
                    $settings['url'] = $atom->file_source;
                    break;

                  default:
                    break;
                }

                $media_url = $settings['url'];
                $settings['media_fid'] = $sid;
                $settings['media_filename'] = check_plain($atom->title);

                $overlay_height = '';
                $overlay_width = '';

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

                $dimensions = array(
                  'width' => $overlay_width,
                  'height' => $overlay_height,
                );

                image_style_transform_dimensions($overlay_style, $dimensions);

                $overlay['height'] = $settings['media_height'] = $dimensions['height'];
                $overlay['width'] = $settings['media_width'] = $dimensions['width'];

                $settings['type'] = $atom->type == 'slick' ? 'image' : $atom->type;

                break;
            }

            $settings['image_style'] = $overlay_style;

            // Image with picture supports, but disabled for small overlay,
            // since we don't provide UI to manage small pictures, and picture
            // is only reasonable for large images.
            $overlay_item = _slick_fields_get_picture($settings, $overlay, $key, $media_url);

            // Pass it over to theme_slick_media.
            $slide['caption']['overlay'] = array(
              '#theme' => 'slick_media',
              '#item' => $overlay_item,
              '#settings' => $settings,
            );

            // If has Media fields.
            if ($settings['is_media']) {
              $slide['caption']['overlay']['#media'] = $media;
            }
          }
        }

        // Title, if so configured.
        if (!empty($settings['slide_title'])) {
          $slide_title = $settings['slide_title'];
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
        if (!empty($settings['slide_caption'])) {
          $caption_items = array();
          foreach ($settings['slide_caption'] as $i => $caption_field) {
            if (isset($fc_fields[$caption_field])) {
              $caption_items[$i] = $fc_fields[$caption_field];
            }
          }

          if ($caption_items) {
            $slide['caption']['data'] = $caption_items;
          }
        }

        // Link, if so configured.
        if (!empty($settings['slide_link']) && isset($fc_fields[$settings['slide_link']])) {
          $slide['caption']['link'] = $fc_fields[$settings['slide_link']];
        }

        // Layout, if so configured.
        if (!empty($settings['slide_layout']) && isset($fc[$settings['slide_layout']][$langcode][0]['value'])) {
          $layout = $fc[$settings['slide_layout']][$langcode][0]['value'];
          $slide['caption']['layout'] = check_plain($layout);
        }

        // Color.
        if (isset($settings['color_field']) && $settings['color_field']) {
          if (isset($fc_fields[$settings['color_field']])) {
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
              $dark = slick_color_brightness($color, -42);
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
      }

      $build[$key] = $slide;
    }
  }

  $settings['inline_css'] = $css;

  return $build;
}
