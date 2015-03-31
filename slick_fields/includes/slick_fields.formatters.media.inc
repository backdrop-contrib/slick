<?php
/**
 * @file
 * Fields formatter for the Slick and Media integration.
 */

/**
 * Format image/media file data.
 */
function _slick_fields_format_media(&$settings, $items, $entity_type, $field, $view_mode, $thumb_nav = FALSE) {
  module_load_include('inc', 'slick', 'includes/slick.extras');

  $build = array();
  $css = array();

  $slick_id = $settings['attributes']['id'];
  $asnavfor = 'main';

  foreach ($items as $key => $item) {
    $slide = array(
      'item' => $item,
      'slide' => array(),
      'caption' => array(),
    );

    $slide_type = isset($slide['item']['type']) ? $slide['item']['type'] : $field['type'];

    // Setup the variables for the image.
    $image_style     = $settings['image_style'];
    $image['uri']    = $slide['item']['uri'];
    $image['width']  = isset($slide['item']['metadata']['width']) ? $slide['item']['metadata']['width'] : NULL;
    $image['height'] = isset($slide['item']['metadata']['height']) ? $slide['item']['metadata']['height'] : NULL;
    $image['alt']    = $slide['item']['alt'];
    $image['title']  = $slide['item']['title'];

    $settings['has_media']    = TRUE;
    $settings['is_media']     = FALSE;
    $settings['media_height'] = $image['height'];
    $settings['media_width']  = $image['width'];

    $file = (object) $slide['item'];
    $uri = $file->uri;

    // If a file has an invalid type, allow file_view_file() to work.
    if (!file_type_is_enabled($file->type)) {
      $file->type = file_get_type($file);
    }

    $media = file_view_file($file, $view_mode);

    if ($media && isset($media['#uri'])) {
      $settings['is_media'] = TRUE;
      if (isset($media['#options'])) {
        $settings['media_height'] = $media['#options']['height'] ? $media['#options']['height'] : 640;
        $settings['media_width'] = $media['#options']['width'] ? $media['#options']['width'] : 360;
      }
      $settings['media_uri'] = $media['#uri'];
      $settings['media_fid'] = $file->fid;
      $settings['filename']  = $file->filename;
    }
    $scheme = file_uri_scheme($uri);
    $image_style = isset($media['#style_name']) ? $media['#style_name'] : $image_style;

    $settings['image_style'] = $image_style;
    $settings['scheme']      = $scheme;
    $settings['type']        = $slide_type;

    // Get audio/video thumbnail uri.
    if ($media_image_uri = _slick_fields_get_media_thumbnail($settings, $uri, $image_style)) {
      $image['uri'] = $media_image_uri;
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
      // Title, if so configured.
      $title = '';
      if ($slide_title = $settings['slide_title']) {
        if ($field_title = field_get_items('file', $file, $slide_title)) {
          // Plain text title.
          if (isset($field_title[0]['safe_value'])) {
            $title = $field_title[0]['safe_value'];
          }
          // Link field.
          elseif (isset($field_title[0]['url'])) {
            // The $item paramater expected here is $field_title[0].
            // Sanitization is handled by theme_link_formatter_link_default().
            $title = field_view_value('file', $file, $slide_title, $field_title[0], $view_mode);
          }

          $slide['caption']['title'] = $title;
        }
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
        $slide['caption']['data'] = $caption_items;
      }

      // Editor links.
      if (file_entity_access('update', $file)) {
        // This is basically to trigger/ bring to translation tab as defined at
        // admin/config/regional/entity_translation/translatable/FIELD_NAME
        if (($title || $caption_items) && field_is_translatable($entity_type, $field)) {
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
      if ($slide_link = $settings['slide_link']) {
        if (field_get_items('file', $file, $slide_link)) {
          $link = field_view_field('file', $file, $slide_link, $view_mode);
          $slide['caption']['link'] = $link;
        }
      }

      // Layout, if so configured.
      if ($settings['slide_layout']) {
        if ($field_layout = field_get_items('file', $file, $settings['slide_layout'])) {
          $slide['caption']['layout'] = check_plain($field_layout[0]['value']);
        }
      }

      $media_url = '';
      if ($settings['is_media']) {
        $rendered_media = render($media);
        $media_url = slick_get_media_url($rendered_media);
        $settings['url'] = $media_url;
      }

      // Image with picture and colorbox supports.
      $image_slide = _slick_fields_get_picture($settings, $image, $key, $media_url);
    }
    else {
      $image_slide = $thumbnail;
      $settings['type'] = 'image';
      $asnavfor = 'thumbnail';

      // Thumbnail captions.
      if ($settings['thumbnail_caption']) {
        if ($thumbnail_caption = field_get_items('file', $file, $settings['thumbnail_caption'])) {
          $slide['caption']['data']['#markup'] = filter_xss($thumbnail_caption[0]['value']);
        }
      }
    }

    // Slide classes, if so configured.
    if ($settings['slide_classes']) {
      if ($slide_classes = field_get_items('file', $file, $settings['slide_classes'])) {
        $settings['classes'] = strip_tags($slide_classes[0]['value']);
      }
    }

    // Pass it over theme_slick_media.
    $slide['slide'] = array(
      '#theme' => 'slick_media',
      '#item' => $image_slide,
      '#settings' => $settings,
      '#asnavfor' => $asnavfor,
    );

    // If has Media fields.
    if ($settings['is_media'] && !$thumb_nav) {
      $slide['slide']['#media'] = $media;
    }

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
