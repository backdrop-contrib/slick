<?php
/**
 * @file
 * Slick options.
 *
 * This file is called by admin and several frontend functions.
 */

/**
 * Defines a list of options available for the Slick.
 *
 * @return array
 *   All available Slick options.
 *
 * @see http://kenwheeler.github.io/slick
 */
function slick_get_options() {
  $options = &drupal_static(__FUNCTION__, NULL);

  if (!isset($options)) {
    $options = array(
      'mobileFirst'       => FALSE,
      'asNavFor'          => '',
      'accessibility'     => TRUE,
      'adaptiveHeight'    => FALSE,
      'autoplay'          => FALSE,
      'autoplaySpeed'     => 3000,
      'pauseOnHover'      => TRUE,
      'pauseOnDotsHover'  => FALSE,
      'arrows'            => TRUE,
      'appendArrows'      => '$(element)',
      'prevArrow'         => '<button type="button" data-role="none" class="slick-prev">Previous</button>',
      'nextArrow'         => '<button type="button" data-role="none" class="slick-next">Next</button>',
      'centerMode'        => FALSE,
      'centerPadding'     => '50px',
      'dots'              => FALSE,
      'dotsClass'         => 'slick-dots',
      'appendDots'        => '$(element)',
      'draggable'         => TRUE,
      'fade'              => FALSE,
      'focusOnSelect'     => FALSE,
      'infinite'          => TRUE,
      'initialSlide'      => 0,
      'lazyLoad'          => 'ondemand',
      'respondTo'         => 'window',
      'rtl'               => FALSE,
      'rows'              => 1,
      'slidesPerRow'      => 1,
      'slide'             => '',
      'slidesToShow'      => 1,
      'slidesToScroll'    => 1,
      'speed'             => 500,
      'swipe'             => TRUE,
      'swipeToSlide'      => FALSE,
      'edgeFriction'      => 0.35,
      'touchMove'         => TRUE,
      'touchThreshold'    => 5,
      'useCSS'            => TRUE,
      'cssEase'           => 'ease',
      'cssEaseBezier'     => '',
      'cssEaseOverride'   => '',
      'easing'            => 'linear',
      'variableWidth'     => FALSE,
      'vertical'          => FALSE,
      'verticalSwiping'   => FALSE,
      'waitForAnimate'    => TRUE,
    );

    // Allows options information to be altered.
    drupal_alter('slick_options_info', $options);
  }

  return $options;
}

/**
 * Defines a list of options available for the responsive Slick.
 *
 * @param int $count
 *   The number of breakpoint if specified.
 *
 * @return array
 *   An array of Slick options copied from the main options with some duplicated
 *   options removal.
 */
function slick_get_responsive_options($count = 0) {
  $options = array();

  $breakpoints = drupal_map_assoc(range(0, ($count - 1)));
  $slick_options = slick_get_options();

  // Clean up problematic dup options.
  $slick_options = slick_clean_options($slick_options);

  foreach ($breakpoints as $key => $breakpoint) {
    $options[$key] = array(
      'breakpoint' => 0,
      'unslick' => FALSE,
      'settings' => array(),
    );

    // Duplicate main relevant settings.
    foreach ($slick_options as $name => $value) {
      $options[$key]['settings'][$name] = $value;
    }
  }

  return $options;
}

/**
 * Returns overridable options to re-use one optionset, only accepts boolean.
 *
 * @return array
 *   An array of overridable boolean options.
 */
function slick_get_overridable_options() {
  $overridables = array(
    'arrows' => t('Arrows'),
    'autoplay' => t('Autoplay'),
    'dots' => t('Dots'),
    'draggable' => t('Draggable'),
  );

  // Allows default settings information to be altered.
  drupal_alter('slick_overridable_options_info', $overridables);

  return $overridables;
}

/**
 * Returns HTML or layout related settings, none of JS, except mousewheel.
 *
 * @param int $count
 *   The number of available slick items.
 *
 * @return array
 *   An array of alterable default settings.
 */
function slick_get_element_default_settings($count = 0) {
  $settings = array(
    'count' => $count,
    'optionset' => 'default',
    'optionset_thumbnail' => '',
    'asnavfor_target' => '',
    'skin' => '',
    'skin_thumbnail' => '',
    'thumbnail_hover' => '',
    'thumbnail_style' => '',
    'skin_arrows' => '',
    'skin_dots' => '',
    'grid' => '',
    'field_type' => '',
    'type' => '',
    'media_switch' => FALSE,
    'is_carousel' => FALSE,
    'has_pattern' => FALSE,
    'has_arrow_down' => FALSE,
    'mousewheel' => FALSE,
    'aspect_ratio_overlay' => FALSE,
    'current_display' => 'main',
  );

  // Allows default settings information to be altered.
  drupal_alter('slick_element_default_settings_info', $settings);

  return $settings;
}

/**
 * Removes problematic options for the responsive Slick.
 *
 * The problematic options are those that should exist once for a given Slick
 *   instance, or no easy way to deal with, or doesn't make sense in the
 *   responsive context.
 *   JS takes care of the relevant copy on each responsive setting instead.
 *
 * @param array $options
 *   An array of all available options.
 *
 * @return array
 *   An array of cleaned out options.
 */
function slick_clean_options(array $options) {
  $cleaned_options = array(
    'mobileFirst' => FALSE,
    'asNavFor' => FALSE,
    'slide' => FALSE,
    'lazyLoad' => FALSE,
    'prevArrow' => FALSE,
    'nextArrow' => FALSE,
    'appendArrows' => FALSE,
    'dotsClass' => FALSE,
    'appendDots' => FALSE,
    'rtl' => FALSE,
    // 'useCSS' => FALSE,
    // 'cssEase' => FALSE,
    // 'cssEaseBezier' => FALSE,
    // 'cssEaseOverride' => FALSE,
    // 'easing' => FALSE,
  );

  // Allows removable responsive options information to be altered.
  drupal_alter('slick_clean_options_info', $cleaned_options);

  return array_diff_key($options, $cleaned_options);
}

/**
 * Applies a very permissive XSS/HTML filter for admin-only use.
 *
 * Allows all tags that can be used inside an HTML body, save for scripts and
 * styles, copied from filter_xss_admin() to allow extending it.
 *
 * @see filter_xss_admin()
 */
function slick_filter_xss_admin($string) {
  $default_tags = array('a', 'abbr', 'acronym', 'address', 'article', 'aside', 'b', 'bdi', 'bdo', 'big', 'blockquote', 'br', 'button', 'canvas', 'caption', 'cite', 'code', 'col', 'colgroup', 'command', 'dd', 'del', 'details', 'dfn', 'div', 'dl', 'dt', 'em', 'figcaption', 'figure', 'footer', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'header', 'hgroup', 'hr', 'i', 'img', 'ins', 'kbd', 'li', 'map', 'mark', 'menu', 'meter', 'nav', 'ol', 'output', 'p', 'pre', 'progress', 'q', 'rp', 'rt', 'ruby', 's', 'samp', 'section', 'small', 'span', 'strong', 'sub', 'summary', 'sup', 'table', 'tbody', 'td', 'tfoot', 'th', 'thead', 'time', 'tr', 'tt', 'u', 'ul', 'var', 'wbr');
  $allowed_tags = variable_get('slick_allowed_tags', $default_tags);

  return filter_xss($string, $allowed_tags);
}

/**
 * Default settings for a newly created option set.
 *
 * @param string $set
 *   Gets a specific group of values [optional]: settings, general, responsives.
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 *
 * @return array
 *   An array of default options, or a specific group if $set is specified.
 */
function _slick_optionset_defaults($set = NULL, $breakpoints = NULL) {
  $slick_options = slick_get_options();
  $defaults = array();
  foreach ($slick_options as $name => $value) {
    $defaults['settings'][$name] = $value;
  }

  // Breakpoint settings.
  if ($breakpoints) {
    $slick_responsive_options = slick_get_responsive_options($breakpoints);
    foreach ($slick_responsive_options as $i => $items) {
      foreach ($items as $name => $setting) {
        switch ($name) {
          case 'breakpoint':
          case 'unslick':
            $defaults['responsives']['responsive'][$i][$name] = $setting;
            break;

          case 'settings':
            foreach ((array) $setting as $key => $value) {
              $defaults['responsives']['responsive'][$i][$name][$key] = $value;
            }
            break;
        }
      }
    }
  }

  $defaults['general']['normal'] = '';
  $defaults['general']['thumbnail'] = '';
  $defaults['general']['template_class'] = '';
  $defaults['general']['goodies'] = array();

  // Typecast the values.
  _slick_typecast_optionset($defaults, $breakpoints);

  // Returns a specific group, or all items.
  return $set && isset($defaults[$set]) ? $defaults[$set] : $defaults;
}

/**
 * Returns the Slick default value by $property.
 *
 * @param string $property
 *   The name of property, e.g.: prevArrow, nexArrow.
 * @param string $group
 *   The name of option group, e.g.: settings, general.
 *
 * @return mixed|FALSE
 *   Returns the default value of the property, otherwise FALSE.
 */
function slick_get_default_value($property, $group = 'settings') {
  $defaults = _slick_optionset_defaults($group);
  return isset($defaults[$property]) ? $defaults[$property] : FALSE;
}

/**
 * Returns the current optionset setting under group 'settings'.
 *
 * @param string $id
 *   The optionset ID.
 * @param string $setting_name
 *   The name of setting.
 * @param string $group
 *   The optionset group: general, settings, responsives.
 *
 * @return mixed|FALSE
 *   The current setting value, or FALSE if none matching.
 */
function slick_optionset_get_setting($id, $setting_name, $group = 'settings') {
  $optionset = slick_optionset_load($id);
  $options = $optionset->options[$group];
  return isset($options[$setting_name]) ? $options[$setting_name] : FALSE;
}

/**
 * Strip out options similar to default values from the optionset options.
 *
 * @param object $optionset
 *   The Optionset object.
 * @param array $customs
 *   An array of handed-over JS options defined using #options property or those
 *   manipulated at theme_slick() dependent on various options.
 * @param array $context
 *   The context to check for before doing alteration, such as module settings.
 *
 * @return array
 *   An array of reduced duplicated Optionset options performance wise.
 */
function _slick_remove_default_optionset_options($optionset, $customs = array(), $context = array()) {
  $config = array();

  $options = $optionset->options;
  $breakpoints = isset($optionset->breakpoints) ? $optionset->breakpoints : 0;
  $defaults = _slick_optionset_defaults('settings', $breakpoints);

  // Clean up the main options.
  $settings = $customs ? $customs : $options['settings'];
  $config = array_diff_assoc($settings, $defaults);

  // Remove wasted dependent options if disabled, empty or not.
  slick_remove_wasted_dependent_options($config);

  // Clean up responsive options if similar to main defaults. JS takes care of
  // the copy based on defaults, so we can avoid dup options in the markups.
  $cleaned_responsives = array();
  if (isset($options['responsives']['responsive'])) {
    $responsives = $options['responsives'];
    foreach ($responsives['responsive'] as $key => $responsive) {
      if (empty($responsives['responsive'][$key]['breakpoint'])) {
        unset($responsives['responsive'][$key]);
      }
      if (isset($responsives['responsive'][$key])) {
        $cleaned_responsives[$key] = $responsive;
      }
    }
    if ($cleaned_responsives) {
      $cleaned = array();
      foreach ($cleaned_responsives as $key => $responsive) {
        $cleaned[$key]['breakpoint'] = $cleaned_responsives[$key]['breakpoint'];

        // Isset check as we are growing from non-existent ones.
        if (isset($cleaned_responsives[$key]['unslick']) && $cleaned_responsives[$key]['unslick']) {
          $cleaned[$key]['settings'] = 'unslick';
          unset($cleaned_responsives[$key]['unslick']);
        }
        else {
          $cleaned[$key]['settings'] = array_diff_assoc($cleaned_responsives[$key]['settings'], $defaults);

          // Remove wasted dependent options if disabled, empty or not.
          slick_remove_wasted_dependent_options($cleaned[$key]['settings']);
        }
      }

      // Be sure to follow JS array.
      $config['responsive'] = $cleaned;
    }
  }

  // Allows options removal information to be altered.
  drupal_alter('slick_remove_options_info', $config, $customs, $options, $context);

  return $config;
}

/**
 * Removes wasted dependent options, even if not empty.
 *
 * @param array $config
 *   An array of Optionset options.
 */
function slick_remove_wasted_dependent_options(array &$config) {
  if (isset($config['autoplay']) && !$config['autoplay']) {
    unset($config['pauseOnHover'], $config['pauseOnDotsHover'], $config['autoplaySpeed']);
  }
  if (isset($config['centerMode']) && !$config['centerMode']) {
    unset($config['centerPadding']);
  }
  if (isset($config['arrows']) && !$config['arrows']) {
    unset($config['prevArrow'], $config['nextArrow'], $config['appendArrows']);
  }
  if (isset($config['dots']) && !$config['dots']) {
    unset($config['dotsClass'], $config['appendDots']);
  }
  if (isset($config['swipe']) && !$config['swipe']) {
    unset($config['swipeToSlide']);
  }
  if (isset($config['vertical']) && !$config['vertical']) {
    unset($config['verticalSwiping']);
  }
  if (isset($config['cssEaseBezier']) && $config['cssEaseBezier']) {
    $config['cssEase'] = $config['cssEaseBezier'];
    unset($config['cssEaseOverride'], $config['cssEaseBezier']);
  }
}

/**
 * Returns the typecast values.
 *
 * With this, the generated JSON object has the right values.
 *
 * @param array $options
 *   An array of Optionset options.
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 */
function _slick_typecast_optionset(&$options = array(), $breakpoints = 0) {
  if (isset($options) && !empty($options)) {
    $slick_options = slick_get_options();
    foreach ($slick_options as $name => $value) {
      if (isset($options['settings'][$name])) {
        if ($cast = _slick_gettype($slick_options[$name])) {
          settype($options['settings'][$name], $cast);
        }
      }
    }

    if (isset($options['responsives']['responsive']) && $breakpoints) {
      $slick_responsive_options = slick_get_responsive_options($breakpoints);
      foreach ($slick_responsive_options as $i => $items) {
        foreach ($items as $name => $item) {
          switch ($name) {
            case 'breakpoint':
              settype($options['responsives']['responsive'][$i][$name], 'int');
              break;

            case 'unslick':
              settype($options['responsives']['responsive'][$i][$name], 'bool');
              break;

            case 'settings':
              foreach ($item as $key => $setting) {
                if (isset($options['responsives']['responsive'][$i][$name][$key])) {
                  if ($cast = _slick_gettype($item[$key])) {
                    settype($options['responsives']['responsive'][$i][$name][$key], $cast);
                  }
                }
              }
              break;

            default:
              break;
          }
        }
      }
    }

    // Allows typecasting information to be altered.
    drupal_alter('slick_typecast_optionset_info', $options, $breakpoints);
  }
}

/**
 * Returns the printed typecast from the given value.
 *
 * @param mixed $value
 *   The given value to check for.
 *
 * @return string
 *   The typecast string value, or FALSE for array type.
 */
function _slick_gettype($value) {
  // Skip array.
  if (is_array($value)) {
    return FALSE;
  }

  return gettype($value);
}

/**
 * Returns supported Slick skins, and additional ones.
 *
 * @return array
 *   An array of available skins.
 */
function slick_skins() {
  $skins = &drupal_static(__FUNCTION__, NULL);

  if (!isset($skins)) {
    // Collects a list of the currently available skins.
    $skins = module_invoke_all('slick_skins_info');

    // Allows skin information to be altered.
    drupal_alter('slick_skins_info', $skins);
  }

  return $skins;
}

/**
 * Returns Slick skin arrows created programatically.
 *
 * @return array
 *   The associative array of available skin arrows.
 */
function slick_arrows() {
  $arrows = &drupal_static(__FUNCTION__, NULL);

  if (!isset($arrows)) {
    // Collects a list of the currently available skin arrows.
    $arrows = module_invoke_all('slick_arrows_info');

    // Allows skin arrows information to be altered.
    drupal_alter('slick_arrows_info', $arrows);
  }

  return $arrows;
}

/**
 * Returns Slick skin dots created programatically.
 *
 * @return array
 *   The associative array of available skin dots.
 */
function slick_dots() {
  $dots = &drupal_static(__FUNCTION__, NULL);

  if (!isset($dots)) {
    // Collects a list of the currently available skin dots.
    $dots = module_invoke_all('slick_dots_info');

    // Allows skin dots information to be altered.
    drupal_alter('slick_dots_info', $dots);
  }

  return $dots;
}

/**
 * Returns supported Slick skins expecting inline CSS background, not images.
 *
 * @return array
 *   An array of available skins expecting inline CSS background.
 */
function slick_inline_css_skins() {
  $inline_css = &drupal_static(__FUNCTION__, NULL);
  if (!isset($inline_css)) {
    $inline_css = array(
      'fullscreen',
      'parallax',
    );

    // Allows inline css skins information to be altered.
    drupal_alter('slick_inline_css_skins_info', $inline_css);
  }

  return $inline_css;
}

/**
 * Create a new optionset object.
 *
 * Note that this function does not save the optionset to the database.
 *
 * @param array $values
 *   The given values to build the optionset if provided.
 *
 * @return object
 *   Returns it as an optionset object.
 *
 * @see slick_optionset_save()
 * @see slick_slick_default_presets()
 */
function slick_optionset_create($values = array()) {
  ctools_include('export');
  $optionset = ctools_export_crud_new('slick_optionset');

  // Set the options to an array.
  $optionset->options = $optionset->options['settings'] = array();

  // Assign specified values.
  if (isset($values['name'])) {
    $optionset->name = $values['name'];
  }

  if (isset($values['label'])) {
    $optionset->label = $values['label'];
  }

  if (isset($values['skin'])) {
    $optionset->skin = $values['skin'];
  }

  $optionset->breakpoints = 0;
  if (isset($values['breakpoints'])) {
    $optionset->breakpoints = $values['breakpoints'];
  }

  if (isset($values['options']) and is_array($values['options'])) {
    $optionset->options = $values['options'];
  }

  // Merge default settings with any given settings.
  $optionset_defaults = _slick_optionset_defaults(NULL, $optionset->breakpoints);
  $optionset->options = $optionset->options + $optionset_defaults;

  return $optionset;
}

/**
 * Fetches all option sets from the storage.
 *
 * @param bool $reset
 *   If TRUE, the static cache of all objects will be flushed prior to
 *   loading all. This can be important on listing pages where items
 *   might have changed on the page load.
 *
 * @return array
 *   The associative array of all optionsets.
 */
function slick_optionset_load_all($reset = FALSE) {
  ctools_include('export');
  $optionsets = ctools_export_crud_load_all('slick_optionset', $reset);
  foreach ($optionsets as $optionset) {
    // Ensure the optionset is typecast after being loaded from storage.
    $breakpoints = isset($optionset->breakpoints) ? $optionset->breakpoints : NULL;
    _slick_typecast_optionset($optionset->options, $breakpoints);
  }

  return $optionsets;
}

/**
 * Fetches the given option set.
 *
 * @param string $id
 *   The optionset ID.
 *
 * @return object
 *   Returns it as an object, or default, if no set could be found.
 */
function slick_optionset_load($id) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $id);

  // Ensures deleted optionset while being used doesn't screw up.
  if (!isset($optionset->name)) {
    // Fall back to 'default' option set.
    $optionset = ctools_export_crud_load('slick_optionset', 'default');
    watchdog('slick', 'Fallback to default optionset.', array(), WATCHDOG_WARNING);
  }

  // Ensure the optionset is typecast after being loaded from storage.
  $breakpoints = isset($optionset->breakpoints) ? $optionset->breakpoints : NULL;
  _slick_typecast_optionset($optionset->options, $breakpoints);

  return $optionset;
}

/**
 * Implements hook_image_default_styles().
 */
function slick_image_default_styles() {
  $styles = array();

  // Default image preset for Slick carousel.
  $styles['slick'] = array(
    'effects' => array(
      array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 1380,
          'height' => 420,
        ),
        'weight' => 0,
      ),
    ),
  );

  return $styles;
}
