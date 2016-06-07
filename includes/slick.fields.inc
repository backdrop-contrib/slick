<?php

/**
 * @file
 * Global functions across Slick field formatters.
 */

/**
 * Gets the thumbnail image.
 */
function slick_get_thumbnail($thumbnail_style = '', $media = array()) {
  $thumbnail = array();
  if (!empty($thumbnail_style)) {
    $thumbnail = array(
      '#theme'      => 'image_style',
      '#style_name' => $thumbnail_style,
      '#path'       => $media['uri'],
    );
    foreach (array('alt', 'height', 'title', 'width') as $data) {
      $thumbnail["#$data"] = isset($media[$data]) ? $media[$data] : NULL;
    }
  }
  return $thumbnail;
}

/**
 * Checks whether a skin expecting inline CSS background, not images.
 *
 * @todo use UI with 'Use CSS background' option instead to reduce logic here.
 */
function slick_get_inline_css_skins($skin = NULL) {
  $skins = slick_skins();
  return empty($skin) ? FALSE : (isset($skins[$skin]['inline css']) && $skins[$skin]['inline css']);
}

/**
 * Builds the inline CSS output for skins with explicit 'inline css' key.
 */
function slick_get_inline_css(array &$settings, array &$items) {
  $css = $settings['inline_css'];
  $css = implode("\n", $css);

  // Pass CSS background to theme_slick().
  $settings['background'] = TRUE;
  drupal_alter('slick_inline_css_info', $css, $items, $settings);

  return array(
    'data'  => $css,
    'type'  => 'inline',
    'group' => CSS_THEME + 1,
    'slick' => 'fields',
  );
}
