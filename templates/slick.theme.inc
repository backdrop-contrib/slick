<?php

/**
 * @file
 * Hooks and preprocess functions for the Slick module.
 */

/**
 * Returns HTML for a slick_wrapper when asNavFor, or cache, in use.
 *
 * @param array $variables
 *   An associative array containing:
 *   - items: An array of slick instances: main and thumbnail slicks.
 *   - settings: HTML related settings.
 *
 * @ingroup themeable
 */
function theme_slick_wrapper(array $variables) {
  $element  = $variables['element'];
  $items    = $element['#items'];
  $settings = isset($element['#settings']) ? $element['#settings'] : array();
  $skin     = isset($settings['skin'])     ? $settings['skin']     : '';
  $nav      = isset($settings['nav'])      ? $settings['nav']      : isset($items[1]);

  $build = drupal_render($items[0]);
  if ($nav) {
    $build .= drupal_render($items[1]);

    $attributes['class'] = array('slick-wrapper', 'slick-wrapper--asnavfor');
    if ($skin && $skin != 'asnavfor') {
      $attributes['class'][] = str_replace('_', '-', 'slick-wrapper--' . $skin);
    }
    return '<div' . drupal_attributes($attributes) . '>' . $build . '</div>';
  }

  return $build;
}

/**
 * Prepares variables for slick templates.
 *
 * Default template: slick.tpl.php.
 *
 * @variables array:
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #items, #settings, #options, #optionset, #attached.
 *     - #settings is set via sub-modules and serves various purposes, and not
 *       related to JS settings, mostly slide layouts or attaching assets.
 *     - #options is set programmatically, or hand-crafted, and only accepts
 *       direct key|value pairs related to JS settings, or an optionset name.
 *     - #optionset, if supplied, ensures the optionset loaded once, and cached.
 */
function template_preprocess_slick(&$variables) {
  $element   = $variables['element'];
  $count     = count($element['#items']);
  $defaults  = slick_get_element_default_settings();
  $settings  = isset($element['#settings']) ? array_merge($defaults, $element['#settings']) : $defaults;
  $customs   = isset($element['#options']) ? $element['#options'] : array();
  $name      = isset($customs['optionset']) ? check_plain($customs['optionset']) : $settings['optionset'];
  $optionset = isset($element['#optionset']) && is_object($element['#optionset']) ? $element['#optionset'] : slick_optionset_load($name);
  $general   = $optionset->options['general'];
  $goodies   = $general['goodies'];
  $js        = $customs ? array_merge($optionset->options['settings'], $customs) : $optionset->options['settings'];
  $skin      = $settings['skin'] ? $settings['skin'] : $optionset->skin;
  $display   = $settings['current_display'];

  // Allows manipulating markups with an enforced unslick.
  $settings['count']   = !empty($settings['count']) ? $settings['count'] : $count;
  $settings['unslick'] = !empty($settings['unslick']) || $settings['count'] == 1;

  if ($display == 'thumbnail') {
    $skin = !empty($settings['skin_thumbnail']) ? $settings['skin_thumbnail'] : $optionset->skin;
  }

  // Prepare attributes.
  $attributes          = &$variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $content_attributes  = &$variables['content_attributes_array'];

  if (isset($settings['attributes']) && is_array($settings['attributes'])) {
    $attributes = drupal_array_merge_deep($attributes, $settings['attributes']);
  }

  if (!isset($attributes['id'])) {
    $id = slick_html_id('slick', $settings['id']);
    $attributes['id'] = $display == 'thumbnail' ? $id . '-thumbnail' : $id;
  }

  // Consistent styling is always needed even for an unslick.
  if ($skin) {
    foreach (array('boxed', 'split') as $key) {
      if ($skin !== $key && strpos($skin, $key) !== FALSE) {
        $attributes['class'][] = 'slick--skin--' . $key;
      }
    }
    $attributes['class'][] = drupal_clean_css_identifier('slick--skin--' . $skin);
    $settings['skin'] = $skin;
  }

  $attributes['class'][] = drupal_clean_css_identifier('slick--optionset--' . $name);
  if (!empty($general['template_class'])) {
    $attributes['class'][] = $general['template_class'];
  }

  // Some settings are only reasonable for the main display, not thumbnail.
  if ($display == 'main') {
    $settings['has_pattern'] = $settings['has_pattern'] ? TRUE : !empty($goodies['pattern']);

    if (!empty($settings['media_switch']) && strpos($settings['media_switch'], 'box') !== FALSE) {
      $swicther = str_replace('-switch', '', $settings['media_switch']);
      $attributes['class'][] = 'slick--' . $swicther;
    }
  }

  // Prevents broken slick when only one item given, or an enforced unslick.
  if (!empty($settings['unslick'])) {
    $attributes['class'][] = 'unslick';
  }
  else {
    $js['randomize'] = !empty($goodies['random']);
    $content_attributes['class'][] = 'slick__slider';
    if (!isset($content_attributes['id'])) {
      $content_attributes['id'] = $attributes['id'] . '-slider';
    }

    // The slider must have the attribute "dir" set to "rtl", if so configured.
    if (!empty($js['rtl'])) {
      $content_attributes['dir'] = 'rtl';
    }

    // Arrows are enforced to allow responsive options hide/show them.
    $tags = array('a', 'em', 'strong', 'button', 'p', 'div', 'i', 'span');
    $settings['prev_arrow']      = filter_xss($js['prevArrow'], $tags);
    $settings['next_arrow']      = filter_xss($js['nextArrow'], $tags);
    $arrow_attributes            = &$variables['arrow_attributes_array'];
    $arrow_attributes['class'][] = 'slick__arrow';
    if (!empty($settings['skin_arrows'])) {
      $skin_arrows = str_replace('_', '-', $settings['skin_arrows']);
      $arrow_attributes['class'][] = 'slick__arrow--' . $skin_arrows;
    }

    // Adds helper class if thumbnail on dots hover provided.
    if (!empty($settings['thumbnail_style']) && !empty($settings['thumbnail_hover'])) {
      $js['dotsClass'] .= ' slick-dots--thumbnail';
    }

    // Adds dots skin modifier class if provided.
    if (!empty($settings['skin_dots'])) {
      $js['dotsClass'] .= ' ' . str_replace('_', '-', 'slick-dots--' . $settings['skin_dots']);
    }

    // Only if asNavFor, enforce clickable thumbnail, otherwise no joy.
    $js['asNavFor'] = empty($settings['asnavfor_target']) ? $js['asNavFor'] : $settings['asnavfor_target'];
    if (!empty($settings['optionset_thumbnail']) && !empty($js['asNavFor'])) {
      if ($display == 'thumbnail') {
        $js['focusOnSelect'] = TRUE;
      }

      $attributes['class'][] = 'slick--display--' . $display;
    }
    // Prevents non-empty, but not-used, asNavFor from breaking the slide.
    // @todo or @nottodo remove any babysitting or failsafe stuff like this one.
    else {
      $js['asNavFor'] = '';
    }

    // focusOnSelect won't work with empty slide value, so add proper selector.
    if (empty($js['slide']) && $js['focusOnSelect']) {
      $js['slide'] = $js['rows'] == 1 && $js['slidesPerRow'] == 1 ? '.slick__slide' : $js['slide'];
    }

    if ($display == 'main') {
      if (!empty($goodies['arrow-down']) && $general['arrow_down_target']) {
        $attributes['class'][]     = 'slick--has-arrow-down';
        $arrow_down['class']       = array('slick-down', 'jump-scroll');
        $arrow_down['data-target'] = $general['arrow_down_target'];
        $arrow_down['data-offset'] = $general['arrow_down_offset'];
        $variables['arrow_down']   = '<button' . drupal_attributes($arrow_down) . '></button>';
      }
    }

    // Add the configuration as JSON object into the slick container.
    $js_data = _slick_remove_default_optionset_options($optionset, $js, $settings);
    if (!isset($content_attributes['data-slick']) && $js_data) {
      $content_attributes['data-slick'] = drupal_json_encode($js_data);
    }
  }

  $variables['settings'] = $settings;

  // Process individual item.
  $variables['items'] = array();
  foreach ($element['#items'] as $delta => $item) {
    $settings['current_item'] = $display;

    $slide = _slick_build_slide($item, $delta, $settings);
    $variables['items'][$delta] = $slide;
    unset($slide);
  }

  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_slick().
 */
function template_process_slick(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
  $variables['arrow_attributes'] = empty($variables['arrow_attributes_array']) ? '' : drupal_attributes($variables['arrow_attributes_array']);
}

/**
 * Reuses individual slide building for slick_item() and slick_grid().
 */
function _slick_build_slide($item, $delta = 0, $settings = array()) {
  $settings = isset($item['settings']) ? array_merge($settings, $item['settings']) : $settings;
  return array(
    '#theme'    => 'slick_item',
    '#item'     => isset($item['slide']) ? $item['slide'] : $item,
    '#caption'  => empty($item['caption']) ? array() : array_filter($item['caption']),
    '#delta'    => $delta,
    '#settings' => $settings,
  );
}

/**
 * Implements hook_preprocess_slick_item().
 */
function template_preprocess_slick_item(&$variables) {
  $element  = $variables['element'];
  $item     = $variables['item'] = $element['#item'];
  $settings = $element['#settings'];

  // Thumbnail may not have settings as it just prints the image.
  $settings = isset($item['#settings']) && is_array($item['#settings']) ? array_merge($settings, $item['#settings']) : $settings;

  // Prepare variables, and remove non-BEM default class.
  $variables['classes_array']  = array_diff($variables['classes_array'], array('slick-item'));
  $variables['wrapper_prefix'] = $variables['wrapper_suffix'] = array();
  $variables['content_prefix'] = $variables['content_suffix'] = array();
  $variables['item_prefix']    = $variables['item_suffix'] = array();

  // Configure attributes for containing elements.
  $attributes          = $content_attributes = array();
  $attributes['class'] = $variables['classes_array'];
  $settings['detroy']  = $settings['current_item'] == 'main' && !empty($settings['grid']) && !empty($settings['unslick']);
  $settings['wrapper'] = $settings['count'] > 1 && $settings['current_item'] != 'grid';

  // 1, 2, 3, 1, 2, 3, drink.
  $attributes['class'] += array('slick__slide', 'slide', 'slide--' . $element['#delta']);

  // Media module has type: image, audio, video, as opposed to field_type.
  if (!empty($settings['type']) && $settings['type'] != 'image') {
    $attributes['class'][] = str_replace('_', '-', 'slide--' . $settings['type']);
  }

  // All slide types -- main, thumbnail, grid, overlay -- may have captions.
  $variables['caption'] = $element['#caption'];
  $variables['slide_pattern'] = '';

  // Title, caption and overlay, or nested media.
  if ($settings['current_item'] != 'thumbnail') {
    // Each slide can have unique, or uniform layout.
    if (!empty($settings['layout'])) {
      $attributes['class'][] = str_replace('_', '-', 'slide--caption--' . $settings['layout']);
    }

    // Split image from captions if we do have captions, and main image.
    if (($variables['caption'] && $item) || (!empty($settings['skin']) && strpos($settings['skin'], '3d') !== FALSE)) {
      $variables['item_prefix'] = '<div class="slide__media">';
      $variables['item_suffix'] = '</div>';
    }

    // If fullwidth or fullscreen, add wrappers to hold caption and overlay.
    if (!empty($settings['skin']) && strpos($settings['skin'], 'full') !== FALSE) {
      $variables['title_prefix'] = '<div class="slide__constrained">';
      $variables['title_suffix'] = '</div>';
    }

    // Exclude lightbox switcher as it has its own pattern DIV within A tag.
    if (!empty($settings['has_pattern']) && strpos($settings['media_switch'], 'box') === FALSE) {
      $variables['slide_pattern'] = '<div class="slide__pattern"></div>';
    }
  }

  // Custom individual slide classes.
  if (!empty($settings['slide_classes'])) {
    $attributes['class'][] = trim($settings['slide_classes']);
  }

  // Do not add divities for a single slick (unslick) to have clean markups.
  if ($settings['wrapper']) {
    $variables['wrapper_prefix'] = '<div' . drupal_attributes($attributes) . '>';
    $variables['wrapper_suffix'] = '</div>';

    if (empty($settings['grid'])) {
      $content_attributes['class'][] = 'slide__content';
      $variables['content_prefix'] = '<div' . drupal_attributes($content_attributes) . '>';
      $variables['content_suffix'] = '</div>';
    }
  }

  $variables['settings'] = $settings;
}

/**
 * Implements hook_preprocess_slick_grid().
 */
function template_preprocess_slick_grid(&$variables) {
  $element  = $variables['element'];
  $settings = $element['#settings'];

  $variables['classes_array'] = array_diff($variables['classes_array'], array('slick-grid'));
  $attributes                 = &$variables['attributes_array'];
  $attributes['class']        = $variables['classes_array'];

  if (empty($settings['unslick'])) {
    $attributes['class'][] = 'slide__content';
  }

  $attributes['class'][] = 'slick__grid';
  $attributes['class'][] = 'block-grid';

  $settings['grid_large'] = $settings['grid'];
  foreach (array('small', 'medium', 'large') as $grid) {
    if ($column = $settings['grid_' . $grid]) {
      $attributes['class'][] = $grid . '-block-grid-' . $column;
    }
  }

  $variables['items'] = array();
  foreach ($element['#items'] as $delta => $item) {
    $settings['current_item'] = 'grid';

    $variables['item_attributes_array'][$delta]['class'] = array('slide__grid', 'grid', 'grid--' . $delta);

    $slide['slide'] = _slick_build_slide($item, $delta, $settings);
    $variables['items'][$delta] = $slide;
    unset($slide);
  }

  $variables['classes_array'] = $attributes['class'];
}

/**
 * Implements hook_process_slick_grid().
 */
function template_process_slick_grid(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
  foreach ($variables['items'] as $delta => $item) {
    $variables['item_attributes'][$delta] = empty($variables['item_attributes_array'][$delta]) ? '' : drupal_attributes($variables['item_attributes_array'][$delta]);
  }
}

/**
 * Implements hook_preprocess_slick_media().
 */
function template_preprocess_slick_media(&$variables) {
  $element    = $variables['element'];
  $settings   = $element['#settings'];
  $item       = $variables['item'] = $element['#item'];
  $media      = isset($item['#item']) ? $item['#item'] : $settings;
  $type       = isset($settings['type']) ? $settings['type'] : 'image';
  $switcher   = $settings['media_switch'] = isset($settings['media_switch']) ? $settings['media_switch'] : FALSE;
  $multimedia = in_array($type, array('video', 'audio'));

  // Prepare variables.
  $variables['is_lightbox'] = $switcher && strpos($switcher, 'box') !== FALSE;
  $variables['alternative_content'] = empty($media['title']) ? '' : filter_xss($media['title']);

  // Configure attributes for containing elements.
  $attributes            = &$variables['attributes_array'];
  $content_attributes    = &$variables['content_attributes_array'];
  $attributes['class']   = $variables['classes_array'];
  $attributes['class'][] = 'media media--' . $type;

  if (isset($settings['content_attributes']) && is_array($settings['content_attributes'])) {
    $content_attributes = drupal_array_merge_deep($content_attributes, $settings['content_attributes']);
  }

  // All media types: image/audio/video files.
  if (!empty($switcher)) {
    $attributes['class'][] = 'media--switch';
    // Lightbox switcher applies to all media types, including core image field.
    if ($variables['is_lightbox'] || $switcher == 'content') {
      $switched = str_replace('-switch', '', $switcher);
      $attributes['class'][] = 'media--switch--' . $switched;
    }
  }

  // Pure CSS responsive iframes/images based on aspect ratio to get
  // consistent display for a mix of image/audio/video at one slick.
  if (!empty($settings['ratio'])) {
    $attributes['class'][] = 'media--ratio--on';
    $attributes['class'][] = str_replace(':', '', 'media--ratio--' . $settings['ratio']);

    if (!empty($media['height']) && $settings['ratio'] == 'fluid') {
      $attributes['style'] = 'padding-bottom: ' . round((($media['height'] / $media['width']) * 100), 2) . '%';
    }
  }

  // The media audio/video files.
  $settings['is_media'] = isset($settings['is_media']) && $multimedia ? $settings['is_media'] : FALSE;
  if ($settings['is_media'] && !empty($media['url'])) {
    $content_attributes['class'][] = 'media__iframe';
    $content_attributes['src'] = empty($settings['iframe_lazy']) ? $media['url'] : 'about:blank';

    // If iframe switch disabled, fallback to just iframe, and remove overlay.
    if (empty($switcher)) {
      $variables['item'] = array();
    }
    else {
      // If no colorbox/photobox, it is an image to iframe switcher.
      if (!$variables['is_lightbox']) {
        $attributes['class'][] = 'media--player media--switch--player';
        $attributes['class'][] = str_replace('_', '-', 'media--' . $media['scheme'] . '-player');
      }

      // Gets consistent with colorbox to share JS manipulation.
      $json = array(
        'type'   => $type,
        'player' => TRUE,
        'scheme' => $media['scheme'],
      );
      $content_attributes['data-media'] = drupal_json_encode($json);
      $content_attributes['data-lazy']  = $media['url'];
    }
  }

  $variables['settings']      = $settings;
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_slick_media().
 */
function template_process_slick_media(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Overrides theme_image with Slick lazy loading.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of image data, which may include "uri", "alt",
 *     "width", "height", "title" and "attributes".
 *   - lazy: A flag whether to use the lazyload, or not.
 *   - image_style: The name of the style to alter the original image.
 *   - url: A string containing the link 'url'.
 *   - attributes: Associative array of attributes to be placed in the img tag.
 *   - options: An array of options, including the 'url' options.
 *
 * @ingroup themeable
 */
function theme_slick_image_lazy(array $variables) {
  foreach (array('attributes', 'item', 'image_style', 'lazy', 'options', 'url') as $key) {
    $default = in_array($key, array('attributes', 'item', 'options')) ? array() : NULL;
    $variables[$key] = isset($variables['element']["#$key"]) ? $variables['element']["#$key"] : $default;
  }

  $build      = '';
  $image      = $variables['item'];
  $options    = &$variables['options'];
  $attributes = $variables['attributes'];

  $attributes['class'][] = 'slick__image';
  $image['path'] = $image['uri'];
  $image_url = file_create_url($image['path']);

  if ($variables['image_style']) {
    $image_url = image_style_url($variables['image_style'], $image['uri']);
    $image['path'] = $image_url;
  }

  $options['image_url'] = $image_url;

  // Be sure to avoid images with breakpoints, taken over by picture.module.
  if (!isset($variables['breakpoints']) && $variables['lazy']) {
    foreach (array('width', 'height', 'alt', 'title') as $key) {
      if (isset($image[$key])) {
        // If the property has already been defined in the attributes,
        // do not override, including NULL.
        if (array_key_exists($key, $attributes)) {
          continue;
        }
        $attributes[$key] = $image[$key];
      }
    }

    // Allows advanced lazyloader to override this without full theme override.
    $lazy_attribute = empty($options['lazy_attribute']) ? 'lazy' : $options['lazy_attribute'];
    $attributes['class'][] = empty($options['lazy_class']) ? 'lazy' : $options['lazy_class'];

    $attributes['src'] = 'data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7';
    $attributes['data-' . $lazy_attribute] = $options['image_url'];

    // Do not pass to theme_image as D7 doesn't support data URI, yet.
    $build .= '<img' . drupal_attributes($attributes) . ' />';
  }
  else {
    $build = theme('image', $image);
  }

  // The link url and link options are both optional.
  if (!empty($variables['url'])) {
    if (isset($options['lightbox']) && $switcher = $options['lightbox']) {
      $build .= empty($options['icon']) ? '<i class="media-icon media-icon--lightbox media-icon--' . $switcher . '"></i>' : $options['icon'];
      if (!empty($options['has_pattern'])) {
        $build .= '<div class="slide__pattern"></div>';
      }
    }

    $link_options = isset($options['url']) ? (array) $options['url'] : array();
    $link_options['html'] = TRUE;
    $build = l($build, $variables['url'], $link_options);
  }

  return $build;
}

/**
 * Returns HTML or layout related settings to avoid extra checks and notices.
 */
function slick_get_element_default_settings() {
  $settings = array(
    'current_display' => 'main',
    'grid'            => '',
    'has_pattern'     => FALSE,
    'id'              => '',
    'media_switch'    => '',
    'nav'             => FALSE,
    'optionset'       => 'default',
    'skin'            => '',
  );
  drupal_alter('slick_element_default_settings_info', $settings);
  return $settings;
}

/**
 * Strips out options similar to default values from the optionset options.
 */
function _slick_remove_default_optionset_options($optionset, $js = array(), $context = array()) {
  $config   = array();
  $options  = $optionset->options;
  $defaults = slick_get_options();

  // Remove wasted dependent options if disabled, empty or not.
  slick_remove_wasted_dependent_options($js);
  $config = array_diff_assoc($js, $defaults);

  if (empty($config['lazyLoad'])) {
    unset($config['lazyLoad']);
  }
  unset($config['prevArrow'], $config['nextArrow']);

  // Clean up responsive options if similar to the defaults.
  $responses = array();
  if (isset($options['responsives']) && isset($options['responsives']['responsive'])) {
    $responsives = $options['responsives']['responsive'];
    foreach ($responsives as $key => $responsive) {
      if (empty($responsives[$key]['breakpoint'])) {
        unset($responsives[$key]);
      }
      if (isset($responsives[$key])) {
        $responses[$key] = $responsive;
      }
    }
    if ($responses) {
      $cleaned = array();
      foreach ($responses as $i => $response) {
        $cleaned[$i]['breakpoint'] = $responses[$i]['breakpoint'];
        if (isset($responses[$i]['unslick']) && $responses[$i]['unslick']) {
          $cleaned[$i]['settings'] = 'unslick';
          unset($responses[$i]['unslick']);
        }
        else {
          slick_remove_wasted_dependent_options($responses[$i]['settings']);
          $cleaned[$i]['settings'] = array_diff_assoc($responses[$i]['settings'], $defaults);
        }
      }
      $config['responsive'] = $cleaned;
    }
  }
  drupal_alter('slick_remove_options_info', $config, $js, $options, $context);
  return $config;
}

/**
 * Removes wasted dependent options, even if not empty.
 */
function slick_remove_wasted_dependent_options(array &$config) {
  $options = array(
    'autoplay'   => array('pauseOnHover', 'pauseOnDotsHover', 'autoplaySpeed'),
    'centerMode' => array('centerPadding'),
    'dots'       => array('dotsClass', 'appendDots'),
    'rows'       => array('slidesPerRow'),
    'swipe'      => array('swipeToSlide'),
    'vertical'   => array('verticalSwiping'),
    'useCSS'     => array('cssEase', 'cssEaseBezier', 'cssEaseOverride'),
  );

  foreach ($options as $key => $option) {
    if (isset($config[$key]) && empty($config[$key])) {
      foreach ($option as $dependent) {
        unset($config[$dependent]);
      }
    }
  }

  if ($config['useCSS'] && !empty($config['cssEaseBezier'])) {
    $config['cssEase'] = $config['cssEaseBezier'];
    unset($config['cssEaseOverride'], $config['cssEaseBezier']);
  }
}
