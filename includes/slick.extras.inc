<?php
/**
 * @file
 * Contains optional functions called by frontend Media, or Field collection.
 */

/**
 * Checks for the supported media sub-modules.
 */
function slick_is_media($scheme) {
  if (in_array($scheme, array('vimeo', 'youtube', 'soundcloud'))) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Gets a media URL from the rendered iframe already containing all the params.
 */
function slick_get_media_url(array &$settings, array &$media, $file = NULL) {
  $url = '';
  if (!slick_is_media($media['scheme'])) {
    return $url;
  }

  // If a file has an invalid type, allow file_view_file() to work.
  if (!file_type_is_enabled($file->type)) {
    $file->type = file_get_type($file);
  }

  if (empty($media['type'])) {
    $media['type'] = $file->type;
  }
  if (empty($settings['view_mode'])) {
    $settings['view_mode'] = 'full';
  }
  $renderables = file_view_file($file, $settings['view_mode']);

  // Only do the hard work if we do have a misconfigured file view mode.
  // This is a fallback for the failing expected output when all weights are 0,
  // and the non-supported schemes may screw up file_view_file().
  if (!isset($renderables['#uri'])) {
    $formatter = 'media_' . $media['scheme'] . '_' . $media['type'];
    $renderables = array(
      '#theme' => $formatter,
      '#uri' => $file->uri,
      '#options' => slick_load_media_settings($media['type'], $settings['view_mode'], $formatter),
    );
  }

  // Allows various iframe dimensions based on the given image_style, good
  // to re-use a single file view mode for various file host entity view modes.
  if (!empty($settings['image_style'])) {
    $dimensions = array(
      'width' => empty($media['width']) ? NULL : $media['width'],
      'height' => empty($media['height']) ? NULL : $media['height'],
    );

    image_style_transform_dimensions($settings['image_style'], $dimensions);

    $media['width'] = $settings['content_attributes']['width'] = $dimensions['width'];
    $media['height'] = $settings['content_attributes']['height'] = $dimensions['height'];
    $media['dimensions'] = TRUE;
  }

  $rendered_media = drupal_render($renderables);

  if (!empty($rendered_media) && strpos($rendered_media, 'src') !== FALSE) {
    $dom = new DOMDocument();
    libxml_use_internal_errors(TRUE);
    $dom->loadHTML($rendered_media);
    $url = $dom->getElementsByTagName('iframe')->item(0)->getAttribute('src');
    $media['url'] = $url;
    $settings['is_media'] = TRUE;
  }
  return $url;
}

/**
 * Format image/media file data, called by Media file and Field collection.
 */
function slick_get_media_thumbnail(array &$settings, $media = array()) {
  $image_uri = '';
  if (!slick_is_media($media['scheme'])) {
    return $image_uri;
  }

  $settings['soundcloud'] = FALSE;
  // Ensures disabling Media sub-modules while being used doesn't screw up.
  try {
    $wrapper = file_stream_wrapper_get_instance_by_uri($media['media_uri']);

    if (!is_object($wrapper)) {
      throw new Exception('Unable to find matching wrapper');
    }

    $parts = $wrapper->get_parameters();
    $media_id = '';
    switch ($media['scheme']) {
      case 'vimeo':
      case 'youtube':
        $media_id = $parts['v'];
        break;

      case 'soundcloud':
        if ($wrapper->interpolateUrl()) {
          $settings['soundcloud'] = TRUE;
        }
        $media_id = $parts['u'];
        break;
    }

    $valid_image_styles = image_style_options(FALSE);
    if (empty($settings['image_style']) || !isset($valid_image_styles[$settings['image_style']])) {
      $image_uri = $wrapper->getOriginalThumbnailPath();
    }
    else {
      $image_uri = $wrapper->getLocalThumbnailPath();
    }

    $settings['media_id'] = $media_id;
    if ($media_id) {
      $media_id = drupal_html_id('media-' . $media['scheme'] . '-' . $media_id);
      $settings['content_attributes']['id'] = $media_id;
    }
  }
  catch (Exception $e) {
    watchdog('slick', 'Unable to render media from %uri. Error: %error', array('%uri' => $media['media_uri'], '%error' => $e->getMessage()));
    return '';
  }
  return $image_uri;
}

/**
 * Returns an array of {file_display} settings for the file type and view mode.
 *
 * It is a clone of file_displays_load() to load only the known/supported ones.
 */
function slick_load_media_settings($type, $view_mode, $formatter) {
  ctools_include('export');

  $name = $type . '__' . $view_mode . '__' . $formatter;
  $displays = ctools_export_load_object('file_display', 'names', array($name));
  return $displays[$name]->settings;
}

/**
 * Builds slide captions with possible multi-value fields.
 */
function slick_get_caption($settings, $entity_type, $entity, $view_mode, array &$slide) {
  // Title can be plain text, or link field.
  if (!empty($settings['slide_title']) && $slide_title = field_get_items($entity_type, $entity, $settings['slide_title'])) {
    if (isset($slide_title[0]['value']) && !empty($slide_title[0]['value'])) {
      // Prevents HTML filter enabled text from having bad markups (h2 > p).
      $slide['caption']['title']['#markup'] = filter_xss($slide_title[0]['value']);
    }
    elseif (isset($slide_title[0]['url']) && !empty($slide_title[0]['title'])) {
      // The $item paramater expected here is $slide_title[0].
      $slide['caption']['title'] = field_view_value($entity_type, $entity, $settings['slide_title'], $slide_title[0], $view_mode);
    }
  }

  // Other caption fields, if so configured.
  if (!empty($settings['slide_caption'])) {
    $caption_items = array();
    foreach ($settings['slide_caption'] as $i => $caption_field) {
      if (!empty($settings['markup'])) {
        $caption_items[$i] = field_view_field($entity_type, $entity, $caption_field, $view_mode);
      }
      elseif ($captions = field_get_items($entity_type, $entity, $caption_field)) {
        foreach ($captions as $j => $caption) {
          $caption_items[$i][$j] = field_view_value($entity_type, $entity, $caption_field, $caption, $view_mode);
        }
      }
    }
    if ($caption_items) {
      $slide['caption']['data'] = $caption_items;
    }
  }

  // Link, if so configured.
  if (!empty($settings['slide_link']) && $slide_links = field_get_items($entity_type, $entity, $settings['slide_link'])) {
    $links = array();
    foreach ($slide_links as $i => $slide_link) {
      $links[$i] = field_view_value($entity_type, $entity, $settings['slide_link'], $slide_link, $view_mode);
    }
    $slide['caption']['link'] = $links;
  }
}

/**
 * Adjusts colors by step/number (0-255).
 *
 * @param string $hex
 *   A string hex value.
 * @param int $steps
 *   An int step between -255 and 255 where negative means darker, and positive
 *   lighter.
 *
 * @return string
 *   A new hex color string value dependent on steps.
 */
function slick_color_brightness($hex, $steps) {
  // Steps should be -255 and 255.
  $steps = max(-255, min(255, $steps));

  // Format the hex color string.
  $hex = str_replace('#', '', $hex);
  if (strlen($hex) == 3) {
    $hex = str_repeat(substr($hex, 0, 1), 2) . str_repeat(substr($hex, 1, 1), 2) . str_repeat(substr($hex, 2, 1), 2);
  }

  // Get decimal values.
  $r = hexdec(substr($hex, 0, 2));
  $g = hexdec(substr($hex, 2, 2));
  $b = hexdec(substr($hex, 4, 2));

  // Adjust number of steps and keep it inside 0 to 255.
  $r = max(0, min(255, $r + $steps));
  $g = max(0, min(255, $g + $steps));
  $b = max(0, min(255, $b + $steps));

  $r_hex = str_pad(dechex($r), 2, '0', STR_PAD_LEFT);
  $g_hex = str_pad(dechex($g), 2, '0', STR_PAD_LEFT);
  $b_hex = str_pad(dechex($b), 2, '0', STR_PAD_LEFT);
  return '#' . $r_hex . $g_hex . $b_hex;
}
