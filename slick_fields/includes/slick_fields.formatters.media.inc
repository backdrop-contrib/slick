<?php
/**
 * @file
 * Fields formatter for the Slick and Media integration.
 */

/**
 * Formats image/media file data.
 */
function _slick_fields_format_media(&$settings, $config, $thumb_nav = FALSE) {
  $view_mode = $settings['view_mode'] ? $settings['view_mode'] : 'full';
  $is_ratio  = !empty($settings['media_switch']) && !empty($settings['aspect_ratio']);
  $settings['lightbox_ready'] = $settings['picture_ready'] = TRUE;

  $build = $css = array();
  foreach ($config['items'] as $key => $item) {
    $file  = (object) $item;
    $uri   = $file->uri;
    $media = array(
      'scheme' => file_uri_scheme($uri),
      'media_uri' => $uri,
    );

    // Build settings for both theme_slick_item() and theme_slick_media().
    $slide = array(
      'item'     => $item,
      'slide'    => array(),
      'settings' => array(
        'count'        => $settings['count'],
        'has_pattern'  => $settings['has_pattern'],
        'image_style'  => $settings['image_style'],
        'layout'       => $settings['slide_layout'],
        'media_switch' => $settings['media_switch'],
        'ratio'        => $is_ratio ? $settings['aspect_ratio'] : FALSE,
        'type'         => $file->type,
        'view_mode'    => $view_mode,
      ),
    );
    slick_extract_image_data($media, $item);
    // Needed settings: type scheme image_style uri view_mode.
    $media['url'] = slick_get_media_url($slide['settings'], $media, $file);

    // Get audio/video thumbnail uri as opposed to the actual (video) uri.
    if ($media_image_uri = slick_get_media_thumbnail($slide['settings'], $media)) {
      $media['uri'] = $media_image_uri;
    }

    // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
    $thumb = empty($settings['thumbnail_style']) ? array() : slick_get_thumbnail($settings['thumbnail_style'], $media);
    if (!$thumb_nav) {
      // Title, can be plain text or link field.
      $title = '';
      if (!empty($settings['slide_title']) && $field_title = field_get_items('file', $file, $settings['slide_title'])) {
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
      if (!empty($settings['slide_caption'])) {
        foreach ($settings['slide_caption'] as $i => $caption_field) {
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
      if (!empty($settings['slide_link']) && field_get_items('file', $file, $settings['slide_link'])) {
        $slide['caption']['link'] = field_view_field('file', $file, $settings['slide_link'], $view_mode);
      }

      // Layout field as opposed from builtin layout above if so configured.
      if (!empty($settings['slide_layout']) && strpos($settings['slide_layout'], 'field_') !== FALSE) {
        $field_layout = field_get_items('file', $file, $settings['slide_layout']);
        $slide['settings']['layout'] = check_plain($field_layout[0]['value']);
      }

      // Slide classes, if so configured.
      if (!empty($settings['slide_classes']) && $slide_classes = field_get_items('file', $file, $settings['slide_classes'])) {
        $slide['settings']['slide_classes'] = strip_tags($slide_classes[0]['value']);
      }

      // Image with picture, lazyLoad, and lightbox supports.
      $slide['slide'] = array(
        '#theme'    => 'slick_media',
        '#item'     => slick_get_image($settings, $media, $key),
        '#settings' => $slide['settings'],
      );
    }
    else {
      $slide['slide'] = $thumb;
      if (!empty($settings['thumbnail_caption']) && $caption = field_get_items('file', $file, $settings['thumbnail_caption'])) {
        $slide['caption']['data']['#markup'] = filter_xss($caption[0]['value']);
      }
    }

    // Fullscreen skin needs inline CSS if so configured.
    if ($config['inline_css']) {
      $css[] = slick_get_inline_css($settings, $slide, $key, $media['uri']);
    }
    $build[$key] = $slide;
  }

  if ($css) {
    $settings['inline_css'] = $css;
  }
  return $build;
}
