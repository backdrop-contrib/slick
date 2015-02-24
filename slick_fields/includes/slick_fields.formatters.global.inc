<?php

/**
 * @file
 * Global functions for Slick fields formatter display.
 */

/**
 * Gets the rendered image based on the Picture mapping, or Slick lazy.
 *
 * It is reasonable to have picture for large images only, while medium to small
 * use lazyLoad instead.
 */
function _slick_fields_get_picture($settings, $image, $key, $media_url = NULL) {
  $build = '';

  // Must be similar for the colorbox rel gallery for image/video/audio.
  $id = isset($settings['colorbox_id']) ? $settings['colorbox_id'] : $settings['attributes']['id'];

  // We are safe to load picture, without relying on function_exists.
  if ($settings['picture_ready'] && $settings['picture'] && $settings['picture_style']) {
    $picture_path     = drupal_get_path('module', 'picture');
    $picture_fallback = $settings['picture_fallback'];
    $mappings         = picture_mapping_load($settings['picture_style']);
    $breakpoints      = picture_get_mapping_breakpoints($mappings, $picture_fallback);

    $image_slide = array(
      '#theme'    => 'picture',
      '#attached' => array(
        'library' => array(
          array('picture', 'picturefill'),
          array('picture', 'picture.ajax'),
        ),
      ),
      '#uri'         => $image['uri'],
      '#height'      => $image['height'],
      '#width'       => $image['width'],
      '#alt'         => $image['alt'],
      '#title'       => $image['title'],
      '#style_name'  => $settings['picture_style'],
      '#attributes'  => array(),
      '#breakpoints' => $breakpoints,
    );

    if ($settings['media_switch'] == 'colorbox-switch' && $settings['colorbox_style']) {
      $image_slide = array(
        '#theme'       => 'picture_formatter_colorbox',
        '#item'        => $image,
        '#image_style' => $settings['picture_style'],
        '#path'        => $image['uri'],
        '#colorbox_image_style' => $settings['colorbox_style'],
        '#colorbox'             => $breakpoints,
        '#colorbox_group_id'    => $id,
        '#attached' => array(
          'css' => array(
            $picture_path . '/picture_colorbox.css' => array('type' => 'file'),
          ),
          'js' => array(
            $picture_path . '/picture_colorbox.js' => array('type' => 'file'),
          ),
        ),
      ) + $image_slide;
    }

    // Render the configured slide.
    $build = render($image_slide);
  }
  else {
    $image_variables = array(
      'delta' => $key,
      'item' => $image,
      'path' => NULL,
      'image_style' => $settings['image_style'],
      'count' => $settings['count'],
      'options' => array(
        'colorbox' => FALSE,
        'type' => $settings['type'],
        'lazy' => $settings['lazy'],
      ),
    );

    // Provide relevant link path if it is a colorbox: media audio/video should
    // provide URL to the actual audio/video URL, image to colorbox image URL.
    if ($settings['colorbox_ready'] && $settings['media_switch'] == 'colorbox-switch' && $settings['colorbox_style']) {
      if ($media_url) {
        $url = $media_url;
      }
      else {
        $url = image_style_url($settings['colorbox_style'], $image['uri']);
      }

      $image_variables['path'] = array(
        'path' => $url,
        'options' => array(
          'attributes' => array(
            'class' => array('slick__colorbox', 'slick__capture'),
          ),
        ),
      );

      $data_media = array('type' => $settings['type']);
      if (isset($settings['scheme'])) {
        $data_media['scheme'] = $settings['scheme'];
      }

      if ($settings['count'] > 1) {
        $data_media['rel'] = $id;
      }

      // Video/audio has special attributes for JS interaction.
      if ($settings['type'] != 'image') {
        $data_media['player'] = TRUE;
        $data_media['height'] = $settings['media_height'];
        $data_media['width'] = $settings['media_width'];
        $image_variables['path']['options']['external'] = TRUE;
      }

      $image_variables['path']['options']['attributes']['data-media'] = drupal_json_encode($data_media);

      $image_variables['options']['colorbox'] = TRUE;
    }

    // Render the configured slide using lazyLoad.
    $build = theme('slick_image_formatter_lazy', $image_variables);
  }

  return $build;
}

/**
 * Format image/media file data.
 */
function _slick_fields_get_media_thumbnail(&$settings, $uri, $image_style = NULL) {
  $image_uri = '';
  $settings['soundcloud'] = FALSE;
  $scheme = $settings['scheme'];
  $scheme_supported = array('vimeo', 'youtube', 'soundcloud');

  if (in_array($settings['type'], array('video', 'audio'))) {
    if (in_array($scheme, $scheme_supported)) {
      $wrapper = file_stream_wrapper_get_instance_by_uri($uri);
      $parts = $wrapper->get_parameters();

      switch ($scheme) {
        case 'vimeo':
        case 'youtube':
          $media_id = $parts['v'];
          break;

        case 'soundcloud':
          // Prevent fatal error for no internet connection.
          if ($wrapper->interpolateUrl()) {
            $settings['soundcloud'] = TRUE;
          }

          $media_id = $parts['u'];
          break;
      }

      $valid_image_styles = image_style_options(FALSE);
      if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
        $image_uri = $wrapper->getOriginalThumbnailPath();
      }
      else {
        $image_uri = $wrapper->getLocalThumbnailPath();
      }

      $settings['media_id'] = $media_id;
    }
  }

  return $image_uri;
}
