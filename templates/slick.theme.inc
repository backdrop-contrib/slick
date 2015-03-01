<?php

/**
 * @file
 * Hooks and preprocess functions for the Slick module.
 */

/**
 * Implements hook_preprocess_slick().
 *
 * Elements:
 * - #settings is set via sub-modules and serves various purposes, and not
 *   related to JS settings, mostly slide layouts or attaching assets.
 * - #options is set programmatically, or hand-crafted, and only accepts direct
 *   key|value pairs related to JS settings, or at least optionset name.
 */
function template_preprocess_slick(&$variables) {
  $element = $variables['element'];
  $items = $element['#items'];
  $count = count($items);

  // HTML or layout related settings, none of JS settings, except mousewheel.
  $settings = array(
    'optionset' => '',
    'optionset_thumbnail' => '',
    'asnavfor_target' => '',
    'skin' => '',
    'skin_thumbnail' => '',
    'grid' => '',
    'media_switch' => FALSE,
    'is_carousel' => FALSE,
    'has_pattern' => FALSE,
    'has_arrow_down' => FALSE,
    'mousewheel' => FALSE,
    'arrows' => FALSE,
    'aspect_ratio_overlay' => FALSE,
    'current_display' => 'main',
    'count' => $count,
  );

  $settings = isset($element['#settings']) ? array_merge($settings, $element['#settings']) : $settings;
  $customs = isset($element['#options']) ? $element['#options'] : array();

  // Optionset may be defined via custom code, CTools UI, or sub-modules.
  $optionset = isset($customs['optionset']) ? $customs['optionset'] : $settings['optionset'];

  // Load the selected optionset.
  if (!empty($optionset)) {
    $optionset = slick_optionset_load($optionset);
  }

  // Check if an optionset was loaded.
  if (empty($optionset)) {
    // Fall back to 'default' option set.
    $optionset = slick_optionset_load('default');
    watchdog('slick', 'Fallback to default optionset.', array(), WATCHDOG_WARNING);
  }

  $options = $optionset->options;
  $general = $options['general'];
  $goodies = $general['goodies'];
  $optionset_name = $optionset->name;

  // The Slick JS settings defined either via custom #options, or CTools UI.
  $js = $customs ? array_merge($options['settings'], $customs) : $options['settings'];

  // Field or Views options override the optionset ones.
  $settings['has_arrow_down'] = $settings['has_arrow_down'] ? TRUE : isset($goodies['arrow-down']) && $goodies['arrow-down'];
  $settings['has_pattern'] = $settings['has_pattern'] ? TRUE : isset($goodies['pattern']) && $goodies['pattern'];

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $content_attributes = $variables['content_attributes_array'];

  // Merge with defined attributes.
  $id = '';
  if (isset($settings['attributes']) && is_array($settings['attributes'])) {
    $attributes += $settings['attributes'];
    $id = isset($attributes['id']) ? $attributes['id'] : '';
  }
  if (isset($settings['content_attributes']) && is_array($settings['content_attributes'])) {
    $content_attributes += $settings['content_attributes'];
  }

  // Set the ID for each slick instance, if none provided, for quick JS lookup.
  // Do not use this dynamic ID to style a slick, use classes instead.
  if (empty($attributes['id'])) {
    $slick_id = &drupal_static('slick_id', 0);
    $id = 'slick-' . ++$slick_id;
    if ($settings['current_display'] == 'thumbnail') {
      $id = $id . '-thumbnail';
    }
    $attributes['id'] = $id;
  }

  $settings['id'] = $id;

  // Since 1.4, Slick is moved to content attributes to allow placing arrows
  // within the slick container, otherwise arrows are slides.
  if (!isset($content_attributes['id'])) {
    $content_attributes['id'] = $id . '-slider';
  }
  $content_attributes['class'][] = 'slick__slider';

  // Add the slick classes.
  if ($count == 1) {
    // Prevent JS errors when only one item given, e.g.: when content editors
    // provide a single image rather than multiple items at their blog.
    $attributes['class'][] = 'unslick';
    $attributes['class'] = array_diff($attributes['class'], array('slick'));
  }
  else {
    // Use this flag to animate the appearance of each slide sequentially using
    // WOW, inView or jquery.appear.
    $settings['is_carousel'] = $js['slidesToShow'] > 1;
    $attributes['class'][] = $settings['is_carousel'] ? 'slick--carousel' : 'slick--slider';

    if (isset($goodies['random']) && $goodies['random']) {
      $attributes['class'][] = 'slick--random';
    }
  }

  $attributes['class'][] = drupal_clean_css_identifier('slick--optionset--' . $optionset_name);

  // Field or Views options override the optionset ones.
  // With asNavFor, skin maybe skin or skin_thumbnail.
  $skin = $settings['skin'] ? $settings['skin'] : $optionset->skin;
  if ($settings['current_display'] == 'thumbnail') {
    $skin = isset($settings['skin_thumbnail']) && $settings['skin_thumbnail'] ? $settings['skin_thumbnail'] : $skin;
  }

  if ($skin) {
    if ($skin !== 'boxed' && strpos($skin, 'boxed') !== FALSE) {
      $attributes['class'][] = 'slick--skin--boxed';
    }
    if ($skin !== 'split' && strpos($skin, 'split') !== FALSE) {
      $attributes['class'][] = 'slick--skin--split';
    }
    $attributes['class'][] = drupal_clean_css_identifier('slick--skin--' . $skin);
  }

  $attributes['class'][] = $js['centerMode'] ? 'slick--center' : 'slick--float';

  // This is needed by Slick media for the aspect ratio option.
  if (isset($settings['field_type'])) {
    $attributes['class'][] = drupal_clean_css_identifier('slick--' . $settings['field_type']);
  }

  // Some settings are only reasonable for the main display, not thumbnail.
  if ($settings['current_display'] == 'main') {
    $attributes['class'][] = 'slick--display--main';
    if ($settings['has_pattern']) {
      $attributes['class'][] = 'slick--has-pattern';
    }

    if ($settings['media_switch'] == 'iframe-switch') {
      if ($settings['aspect_ratio']) {
        $attributes['class'][] = $settings['aspect_ratio_overlay'] ? 'slick--ratio-overlay' : 'slick--ratio';
        $attributes['class'][] = drupal_clean_css_identifier('slick--ratio--' . $settings['aspect_ratio']);
        $attributes['class'][] = drupal_clean_css_identifier('slick--ratio--' . $settings['field_type']);
      }
    }

    if ($settings['has_arrow_down']) {
      $attributes['class'][] = 'slick--has-arrow-down';

      $settings['arrow_down_target'] = '';
      if ($general['arrow_down_target']) {
        $settings['arrow_down_target'] = str_replace(array('#', '.'), '', $general['arrow_down_target']);
      }
      $settings['arrow_down_offset'] = $general['arrow_down_offset'] ? $general['arrow_down_offset'] : 125;
    }
  }
  elseif ($settings['current_display'] == 'thumbnail') {
    $attributes['class'][] = 'slick--display--thumbnail';
  }

  if (!empty($general['template_class'])) {
    $attributes['class'][] = strip_tags($general['template_class']);
  }

  // Provide context to fix excessive height with lazyLoad ondemand.
  if ($js['lazyLoad']) {
    $attributes['class'][] = drupal_clean_css_identifier('slick--' . $js['lazyLoad']);
  }

  // focusOnSelect will not work with empty slide value, so add proper selector.
  // Intentionaly empty the fallback, not using the useless old selector "div"
  // anymore to use the new blank default, and clean up old JSON object.
  // @see https://github.com/kenwheeler/slick/issues/789
  $js['slide'] = !$js['slide'] && $js['focusOnSelect'] ? '.slick__slide' : '';

  // Only add asNavFor if we do have thumbnail optionset.
  // Implementor is now responsible to pass the proper asnavfor_target.
  if ($settings['optionset_thumbnail'] && $settings['asnavfor_target']) {
    // Enforce clickable thumbnail, and solves old lingering "div".
    if ($settings['current_display'] == 'thumbnail') {
      $js['slide'] = '.slick__slide';
      $js['focusOnSelect'] = TRUE;
    }
    $js['asNavFor'] = $settings['asnavfor_target'];
  }
  // Prevents non-empty, but not-used, asNavFor from breaking the slide.
  else {
    $js['asNavFor'] = '';
  }

  // The slider must have the attribute "dir" set to "rtl", if so configured.
  if ($js['rtl']) {
    $content_attributes['dir'] = 'rtl';
  }

  // Arrows must be set to allow responsive options hide/show them.
  // Limit sanitization with button and a few reasonable selectors.
  $settings['arrows'] = $js['arrows'];
  $allowed_tags = array('a', 'em', 'strong', 'button', 'p', 'div', 'i', 'span');
  $is_prev = isset($js['prevArrow']) && $js['prevArrow'];
  $is_next = isset($js['nextArrow']) && $js['nextArrow'];

  $default_prev = slick_get_default_value('prevArrow');
  $default_next = slick_get_default_value('nextArrow');

  $settings['prev_arrow'] = $is_prev ? filter_xss($js['prevArrow'], $allowed_tags) : $default_prev;
  $settings['next_arrow'] = $is_next ? filter_xss($js['nextArrow'], $allowed_tags) : $default_next;

  $js['mousewheel'] = $settings['mousewheel'] ? TRUE : FALSE;

  // Add the configuration as JSON object into the slick container.
  // data-config is kept rather than the new data-slick, otherwise Slick will
  // be initialized too early, causing other functions fail. data-slick is very
  // useful if you have no extra needs.
  // @see https://github.com/kenwheeler/slick/pull/407
  if ($count > 1 && !isset($content_attributes['data-config'])) {
    $js_data = _slick_remove_default_optionset_options($optionset, $js);

    // Remove empty lazyLoad, or left to default ondemand, to avoid JS error.
    if (empty($js_data['lazyLoad'])) {
      unset($js_data['lazyLoad']);
    }

    // Do not pass arrows HTML to JSON object, as we process separately above.
    unset($js_data['prevArrow'], $js_data['nextArrow']);
    $content_attributes['data-config'] = drupal_json_encode($js_data);
  }

  // Process individual item.
  $build = array();
  foreach ($items as $delta => $item) {
    $settings['current_item'] = $settings['current_display'];

    $build[$delta] = array(
      '#theme' => 'slick_item',
      '#item' => isset($item['slide']) ? $item['slide'] : $item,
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  // Add the wrapper attributes.
  $variables['attributes_array'] = $attributes;
  $variables['content_attributes_array'] = $content_attributes;
  $variables['items'] = $build;
  $variables['settings'] = $settings;

  // Populate classes_array expected by Omega 3.
  $variables['classes_array'] = $variables['attributes_array']['class'];

  // Add the basic library if no sub-modules, nor #attached in use.
  if ($count > 1 && !isset($element['#attached']) && ($settings['current_display'] == 'main')) {
    slick_add();
  }
}

/**
 * Implements hook_process_slick().
 */
function template_process_slick(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
  $variables['content_attributes'] = empty($variables['content_attributes_array']) ? '' : drupal_attributes($variables['content_attributes_array']);
}

/**
 * Implements hook_preprocess_slick_item().
 */
function template_preprocess_slick_item(&$variables) {
  $element = $variables['element'];
  $item = $element['#item'];
  $settings = isset($element['#settings']) ? $element['#settings'] : array();
  // $item is not always an array such as from views. This may results in
  // unexpected "&lt;" for custom classes below when checking $item_settings.
  $item_settings = is_array($item) && isset($item['#settings']) ? $item['#settings'] : $settings;
  $captions = $element['#caption'];

  $variables['item'] = $item;
  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  // Remove non-BEM default class.
  $variables['classes_array'] = array_diff($variables['classes_array'], array('slick-item'));

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];

  $variables['tag'] = $variables['content_tag'] = 'div';

  if ($settings['current_item'] == 'grid') {
    $one = 'slide__grid';
    $two = 'grid';
    $three = 'grid--' . $element['#delta'];
    $variables['content_attributes_array']['class'][] = 'grid__content';

    // Borrows Foundation block-grid so that reusable site wide.
    // Override/empty skin Grid css when Foundation grid already in place, and
    // block-grid classes are maintained.
    // @see hook_slick_skins_info_alter(), or other ways to override CSS.
    $variables['tag'] = 'li';
    $variables['content_tag'] = 'div';
  }
  else {
    $one = 'slick__slide';
    $two = 'slide';
    // Useful to add custom text color based on delta.
    $three = 'slide--' . $element['#delta'];
    $variables['content_attributes_array']['class'][] = 'slide__content';
    $variables['tag'] = 'div';
  }

  // 1, 2, 3, 1, 2, 3, drink.
  $attributes['class'][] = $one;
  $attributes['class'][] = $two;
  $attributes['class'][] = $three;

  // Only add relevant Foundation block-grid classes if the skin is Grid so that
  // you can use own grid system, otherwise overrides at several levels.
  if ($settings['grid'] && $settings['current_item'] == 'main') {
    // Allows semantic grid.
    if ($settings['skin'] == 'grid') {
      if ($settings['grid_small']) {
        $variables['content_attributes_array']['class'][] = 'small-block-grid-' . $settings['grid_small'];
      }
      if ($settings['grid_medium']) {
        $variables['content_attributes_array']['class'][] = 'medium-block-grid-' . $settings['grid_medium'];
      }
      $variables['content_attributes_array']['class'][] = 'large-block-grid-' . $settings['grid'];
    }

    $attributes['class'][] = 'slide--grid';

    $variables['content_tag'] = 'ul';
  }

  // Main image/background. Media module has type: image, audio, video, etc.
  // as opposed to field_type.
  if (isset($settings['type'])) {
    $attributes['class'][] = drupal_clean_css_identifier('slide--' . $settings['type']);
  }

  // All slides - main, thumbnail, grid - may have captions.
  $variables['caption'] = array();
  $variables['editor'] = array();
  $variables['slide_pattern'] = '';

  // Title, caption and overlay, or nested media.
  $stage = array();

  if ($settings['current_item'] == 'thumbnail') {
    // Thumbnail caption.
    if (isset($captions['data']) && !empty($captions['data'])) {
      $stage['data'] = $captions['data'];
    }
  }
  else {
    // Add layout/ caption position classes if so configured.
    if (!empty($captions['layout'])) {
      $layout = drupal_clean_css_identifier('slide--caption--' . $captions['layout']);
      $attributes['class'][] = $layout;
    }

    // Overlay image/ video.
    if (!empty($captions['overlay'])) {
      $stage['overlay'] = $captions['overlay'];
    }

    // Title.
    if (!empty($captions['title'])) {
      $stage['title'] = $captions['title'];
    }

    // Core image alt as description.
    if (!empty($captions['alt'])) {
      $stage['alt'] = $captions['alt'];
    }

    // Captions, can be multiple fields, useful for custom markups, or nested
    // slick if possible at all.
    if (!empty($captions['data'])) {
      $stage['data'] = $captions['data'];
    }

    // Link/ buttons.
    if (!empty($captions['link'])) {
      $stage['link'] = $captions['link'];
    }

    // Editor links, implementors should do the access check properly, e.g.:
    // Translate file @title, Edit file @title.
    // @see _slick_fields_format_media()
    if (isset($captions['editor']) && !empty($captions['editor'])) {
      $variables['editor'] = $captions['editor'];
    }

    // We split image from captions with a wrapper if we do have captions, and
    // main image. This allows more complex slide layout.
    if ((!empty($stage) && $item) || (strpos($settings['skin'], '3d') !== FALSE)) {
      $variables['item_prefix'] = '<div class="slide__media">';
      $variables['item_suffix'] = '</div>';
    }

    // If fullwidth or fullscreen, add additional wrappers to hold caption and
    // overlay properly within the max-width containner.
    if (strpos($settings['skin'], 'full') !== FALSE) {
      $variables['title_prefix'] = '<div class="slide__constrained">';
      $variables['title_suffix'] = '</div>';
    }

    // Exclude colorbox switcher as it has its own pattern DIV within A tag.
    // @see theme_slick_image_lazy().
    if (isset($settings['has_pattern']) && $settings['has_pattern'] && $settings['media_switch'] != 'colorbox-switch') {
      $variables['slide_pattern'] = '<div class="slide__pattern"></div>';
    }
  }

  // Pass the captions to template.
  $variables['caption'] = $stage;

  if (isset($item_settings['classes']) && $item_settings['classes']) {
    $attributes['class'][] = strip_tags($item_settings['classes']);
  }

  $variables['settings'] = $settings;
  $variables['attributes_array'] = $attributes;

  // Populate classes_array expected by Omega 3.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_slick_item().
 */
function template_process_slick_item(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Implements hook_preprocess_slick_grid().
 *
 * Tips:
 *   - Avoid variableWidth and adaptiveHeight.
 *   - Use slidesToSHow > 1 to build more complex grids.
 */
function template_preprocess_slick_grid(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'];

  $build = array();

  // Implementors should be responsible to pass proper grid chunks.
  foreach ($element['#items'] as $delta => $item) {
    $settings['current_item'] = 'grid';

    $build[$delta] = array(
      '#theme' => 'slick_item',
      '#item' => isset($item['slide']) ? $item['slide'] : '',
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  $variables['items'] = $build;
}

/**
 * Implements hook_preprocess_slick_media().
 */
function template_preprocess_slick_media(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'];
  $variables['item'] = $element['#item'];
  // Views has no support for #asnavfor since it has no raw to work with yet.
  $variables['asnavfor'] = isset($element['#asnavfor']) ? $element['#asnavfor'] : 'main';

  $type = isset($settings['type']) ? $settings['type'] : 'image';

  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $attributes['class'][] = 'media';

  $variables['url'] = '';
  $variables['alternative_content'] = '';

  // Only reasonable for the main or grid display, not thumbnail navigation.
  if ($variables['asnavfor'] != 'thumbnail') {

    // Specific to Media integration.
    $media = isset($element['#media']) ? $element['#media'] : '';

    if ($media) {
      // Cherry-pick useful references for settings and options.
      $scheme = $settings['scheme'];
      $settings['api_id_attribute'] = drupal_html_id('media-' . $scheme . '-' . $settings['media_id']);

      // Soundcloud has no #options.
      if (isset($media['#options'])) {
        if (($scheme == 'youtube' && $media['#options']['enablejsapi']) || ($scheme == 'vimeo' && $media['#options']['api'])) {
          $variables['content_attributes_array']['id'] = $settings['api_id_attribute'];
        }
      }

      $variables['id'] = $settings['media_fid'];

      // Iframe attributes.
      $variables['content_attributes_array']['class'][] = 'media__iframe';
      $variables['content_attributes_array']['height'] = $settings['media_height'];
      $variables['content_attributes_array']['width'] = $settings['media_width'];

      // Non-JS fallback.
      // The media URL contains all the params extracted from the iframe SRC.
      if ($settings['url']) {
        $variables['url'] = $settings['url'];
        $variables['content_attributes_array']['src'] = $variables['url'];
      }

      // If iframe switch disabled, fallback to just iframe, and remove image
      // overlay.
      if (empty($settings['media_switch'])) {
        $variables['item'] = '';
      }
      else {
        $attributes['class'][] = 'media--switch';

        // If no colorbox, it is an image to iframe switcher.
        if ($settings['media_switch'] != 'colorbox-switch') {
          $attributes['class'][] = 'media--player';
          $attributes['class'][] = 'media--switch--player';
          $attributes['class'][] = drupal_clean_css_identifier('media--' . $scheme . '-player');
        }

        // Gets consistent with colorbox to share JS manipulation.
        $data_media = array('type' => $type);
        $data_media['player'] = TRUE;
        $data_media['scheme'] = $scheme;
        // Add iframe attributes.
        $variables['content_attributes_array']['data-media'] = drupal_json_encode($data_media);
        $variables['content_attributes_array']['data-lazy'] = $variables['url'];
      }
      $variables['alternative_content'] = check_plain($settings['media_filename']);
    }

    // Colorbox switcher applies to all media types, including core image field.
    if ($settings['media_switch'] == 'colorbox-switch') {
      $attributes['class'][] = 'media--switch--colorbox';
    }
  }

  $attributes['class'][] = 'media--' . $type;

  $variables['settings'] = $settings;
  $variables['attributes_array'] = $attributes;

  // Populate classes_array expected by Omega 3.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_slick_media().
 */
function template_process_slick_media(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Implements hook_preprocess_slick_wrapper().
 *
 * @see theme_slick_wrapper()
 */
function template_preprocess_slick_wrapper(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'] ? $element['#settings'] : array();

  $variables['items'] = array();
  foreach ($element['#items'] as $delta => $item) {
    if (!empty($item)) {
      $variables['items'][$delta] = $item;
    }
  }

  $variables['attributes_array'] = array('class' => array('slick-wrapper'));

  if (isset($settings['skin']) && $settings['skin']) {
    $variables['attributes_array']['class'][] = drupal_clean_css_identifier('slick-wrapper--' . $settings['skin']);
  }

  if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
    $variables['attributes_array']['class'][] = 'slick-wrapper--asnavfor';
  }

  // Populate classes_array expected by Omega 3.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_slick_wrapper().
 */
function template_process_slick_wrapper(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Returns HTML for a slick_wrapper.
 *
 * @param array $variables
 *   An associative array containing:
 *   - items: An array of slick items.
 *   - classes: A string containing the classes for the wrapping div.
 *   - attributes: A string containing the attributes for the wrapping div.
 *
 * @see template_preprocess_slick_wrapper()
 * @see template_process_slick_wrapper()
 *
 * @ingroup themeable
 */
function theme_slick_wrapper(array $variables) {
  $build = '';

  // Render the items.
  foreach ($variables['items'] as $delta => $item) {
    $build .= drupal_render($item);
  }

  // Render the top-level DIV.
  $build = '<div' . $variables['attributes'] . '>' . $build . '</div>';

  return $build;
}

/**
 * Overrides theme_image with lazy loading.
 *
 * Notes:
 * The issue here is no longer valid with the latest Slick. However if the issue
 * persists at your end, please try two possible fixes below.
 *
 * v1.3.7 allows dummy/ placeholder image:
 * https://github.com/kenwheeler/slick/pull/223
 * However lazyLoad ondemand has issue failing to generate _newly created image
 * derivatives, causing 403. It has no issue if derivatives are in place
 * already. Only _new images fail. lazyLoad progressive has no issue with it.
 *
 * May relate to itok: https://www.drupal.org/drupal-7.20-release-notes
 * Possible fixes without compromising security:
 *   - https://www.drupal.org/project/imageinfo_cache, to have derivatives
 *     in place earlier.
 *   - Or use lazyLoad "progressive" instead.
 *
 * ondemand also has issue with dummy image excessive height, so use it
 * with care. Dummy image is for valid HTML5.
 *
 * @ingroup themeable
 */
function theme_slick_image_lazy($variables) {
  $build = '';

  $path = $variables['path'];
  $variables['attributes']['class'][] = 'slick__image';

  // Be sure to avoid images with breakpoints and without image style.
  if (!isset($variables['breakpoints']) && isset($variables['style_name'])) {
    $attributes = $variables['attributes'];
    foreach (array('width', 'height', 'alt', 'title') as $key) {
      if (isset($variables[$key])) {
        // Backported https://www.drupal.org/node/999338.
        // If the property has already been defined in the attributes,
        // do not override, including NULL.
        if (array_key_exists($key, $attributes)) {
          continue;
        }
        $attributes[$key] = $variables[$key];
      }
    }

    $variables['path'] = image_style_url($variables['style_name'], $path);
    $count = $variables['count'];
    $path = file_create_url($variables['path']);

    // Provide SRC attribute if a single image, or lazyLoad disabled.
    if ($count == 1 || !$variables['lazy']) {
      $attributes['src'] = $path;
    }
    else {
      $attributes['class'][] = 'lazy';

      // This is why we have this function, D7 core theme_image() only expects
      // a file, not inline image. Dropped at D8 that supports it, and merged to
      // theme_slick_image_formatter_lazy() instead, or vice versa.
      // @see https://www.drupal.org/node/1342504
      $attributes['src'] = 'data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7';
      $attributes['data-lazy'] = $path;
    }

    $build .= '<img ' . drupal_attributes($attributes) . ' />';

    // Only provide noscript to the first image, since the rest of the slides
    // always want JS with all this lazyLoad.
    if ($count > 1 && $variables['delta'] == 0) {
      $build .= '<noscript>' . theme_image($variables) . '</noscript>';
    }
  }
  else {
    $build = theme_image($variables);
  }

  return $build;
}

/**
 * Returns HTML for an image field formatter using lazyLoad technique.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of image data, which may include "uri", "alt",
 *     "width", "height", "title" and "attributes".
 *   - image_style: An optional image style.
 *   - path: An array containing the link 'path' and link 'options'.
 *
 * @ingroup themeable
 */
function theme_slick_image_formatter_lazy(array $variables) {
  $item = $variables['item'];
  $output = '';
  $image = array(
    'path' => $item['uri'],
  );

  if (isset($item['alt'])) {
    $image['alt'] = $item['alt'];
  }

  if (isset($item['attributes'])) {
    $image['attributes'] = $item['attributes'];
  }

  // Do not output an empty 'title' attribute.
  if (isset($item['title']) && drupal_strlen($item['title']) > 0) {
    $image['title'] = $item['title'];
  }

  if ($variables['image_style']) {
    $image['style_name'] = $variables['image_style'];
    $dimensions = array(
      'width' => $item['width'],
      'height' => $item['height'],
    );

    image_style_transform_dimensions($image['style_name'], $dimensions);

    $image['width'] = $dimensions['width'];
    $image['height'] = $dimensions['height'];
  }

  $image['delta'] = $variables['delta'];
  $image['count'] = isset($variables['count']) ? $variables['count'] : 0;

  // Optional lazyLoad only reasonable if we do have slides.
  $image['lazy'] = FALSE;
  if ($image['count'] > 1) {
    $image['lazy'] = $variables['options']['lazy'];
  }
  $image['options']['type'] = $variables['options']['type'];

  $output = theme('slick_image_lazy', $image);

  // The link path and link options are both optional, but for the options to be
  // processed, the link path must at least be an empty string.
  if (isset($variables['path']['path'])) {
    if (isset($variables['options']['colorbox']) && $variables['options']['colorbox']) {
      $output .= '<i class="media-icon media-icon--colorbox"></i>';
      $output .= '<div class="slide__pattern"></div>';
    }

    $path = $variables['path']['path'];
    $options = isset($variables['path']['options']) ? $variables['path']['options'] : array();
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }

  return $output;
}
