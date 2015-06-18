<?php
/**
 * @file
 * Fields formatter for the Slick and Media integration.
 */

/**
 * Formats image/media file data.
 */
function _slick_fields_format_media(&$settings, $config, $thumb_nav = FALSE) {
  $view_mode             = $settings['view_mode'] ? $settings['view_mode'] : 'full';
  $settings['has_media'] = $settings['lightbox_ready'] = $settings['picture_ready'] = TRUE;
  $slick_id              = $settings['attributes']['id'];
  $current_display       = 'main';

  $build = $css = array();
  foreach ($config['items'] as $key => $item) {
    $file   = (object) $item;
    $uri    = $file->uri;
    $scheme = file_uri_scheme($uri);

    // If a file has an invalid type, allow file_view_file() to work.
    if (!file_type_is_enabled($file->type)) {
      $file->type = file_get_type($file);
    }

    // Check for theme_media_youtube_video() et all to avoid extra logic.
    $media    = file_view_file($file, $view_mode);
    $is_media = $media && isset($media['#uri']);

    // Setup the variables, uri can be image, video, or audio uri.
    foreach ($config['data'] as $data) {
      $image[$data] = isset($item[$data]) ? $item[$data] : NULL;
    }

    // Image dimensions are not always available, such as for video and audio.
    foreach (array('width', 'height') as $data) {
      $image[$data] = isset($item['metadata'][$data]) ? $item['metadata'][$data] : (isset($item[$data]) ? $item[$data] : NULL);
      if ($is_media && empty($image[$data])) {
        // Supports non-aspect-ratio iframes to have proper dimensions.
        $image[$data] = isset($media['#options'][$data]) ? $media['#options'][$data] : NULL;
      }
    }

    // Build settings for both theme_slick_item() and theme_slick_media().
    $slide = array(
      'item'     => $item,
      'caption'  => array(),
      'settings' => array(
        'count'        => $settings['count'],
        'image_style'  => $settings['image_style'],
        'media_switch' => $settings['media_switch'],
        'has_pattern'  => $settings['has_pattern'],
        'scheme'       => $scheme,
        'layout'       => $settings['slide_layout'],
        'view_mode'    => $view_mode,
        'ratio'        => $settings['media_switch'] && $settings['aspect_ratio'] ? $settings['aspect_ratio'] : FALSE,
      ),
    );

    // Clone image info to individual theme_slick_media() settings.
    foreach ($image as $i => $value) {
      $slide['settings'][$i] = $image[$i];
    }

    if ($is_media) {
      $slide['settings']['media_uri'] = $media['#uri'];
      $slide['settings']['is_media']  = TRUE;
    }

    // Slide classes, if so configured.
    if ($settings['slide_classes'] && $slide_classes = field_get_items('file', $file, $settings['slide_classes'])) {
      $slide['settings']['classes'] = strip_tags($slide_classes[0]['value']);
    }

    // Get audio/video thumbnail uri as opposed to the actual (video) uri.
    if ($media_image_uri = slick_get_media_thumbnail($slide['settings'], $uri)) {
      $image['uri'] = $media_image_uri;
      if (!empty($slide['settings']['media_id'])) {
        $media_id = drupal_html_id('media-' . $scheme . '-' . $slide['settings']['media_id']);
        $slide['content_attributes']['id'] = $slide['settings']['api_id_attribute'] = $media_id;
      }
    }

    // Provide thumbnail pagers if so configured.
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
      // Title, can be plain text or link field.
      $title = '';
      if ($settings['slide_title'] && $field_title = field_get_items('file', $file, $settings['slide_title'])) {
        if (isset($field_title[0]['safe_value'])) {
          $title = $field_title[0]['safe_value'];
        }
        elseif (isset($field_title[0]['url'])) {
          // The $item paramater expected here is $field_title[0].
          $title = field_view_value('file', $file, $settings['slide_title'], $field_title[0], $view_mode);
        }
        $slide['caption']['title'] = $title;
      }

      // Other caption fields, if so configured.
      $caption_items = array();
      if ($slide_caption = $settings['slide_caption']) {
        foreach ($slide_caption as $i => $caption_field) {
          $is_caption = field_get_items('file', $file, $caption_field);
          if ($is_caption && ($caption = field_view_field('file', $file, $caption_field, $view_mode))) {
            $caption_items[$i] = $caption;
          }
        }
        if ($caption_items) {
          $slide['caption']['data'] = $caption_items;
        }
      }

      // Editor links.
      if (file_entity_access('update', $file)) {
        // This is basically to trigger/ bring to translation tab as defined at
        // admin/config/regional/entity_translation/translatable/FIELD_NAME
        if (($title || $caption_items) && field_is_translatable($config['entity_type'], $config['field'])) {
          $slide['caption']['editor']['translate']['#markup'] = l(t('Translate'), 'file/' . $file->fid . '/translate', array(
            'query' => drupal_get_destination(),
            'attributes' => array(
              'title' => t('Translate file @title', array('@title' => $file->filename)),
              'class' => array('file__translate edit-link icon-globe'),
            ),
          ));
        }

        $slide['caption']['editor']['edit']['#markup'] = l(t('Edit'), 'file/' . $file->fid . '/edit', array(
          'query' => drupal_get_destination(),
          'attributes' => array(
            'title' => t('Edit file @title', array('@title' => $file->filename)),
            'class' => array('file__edit edit-link icon-edit'),
          ),
        ));
      }

      // Link, if so configured.
      if ($settings['slide_link'] && field_get_items('file', $file, $settings['slide_link'])) {
        $slide['caption']['link'] = field_view_field('file', $file, $settings['slide_link'], $view_mode);
      }

      // Layout field as opposed from builtin layout above if so configured.
      if (strpos($settings['slide_layout'], 'field_') !== FALSE && $field_layout = field_get_items('file', $file, $settings['slide_layout'])) {
        $slide['settings']['layout'] = check_plain($field_layout[0]['value']);
      }

      $media_url = '';
      if ($is_media) {
        $rendered_media           = render($media);
        $media_url                = slick_get_media_url($rendered_media);
        $slide['settings']['url'] = $media_url;
      }

      // Image with picture and colorbox supports.
      $image_slide = slick_get_image($settings, $slide['settings'], $image, $key, $media_url);
    }
    else {
      $image_slide               = $thumbnail;
      $slide['settings']['type'] = 'image';
      $current_display           = 'thumbnail';

      // Thumbnail captions.
      if ($settings['thumbnail_caption'] && $caption = field_get_items('file', $file, $settings['thumbnail_caption'])) {
        $slide['caption']['data']['#markup'] = filter_xss($caption[0]['value']);
        $slide['settings']['type'] = empty($thumbnail) ? 'text' : 'image';
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
