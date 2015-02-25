<?php

/**
 * @file
 * Slick elements.
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
    $options = array();

    $options['mobileFirst'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['asNavFor'] = array(
      'cast' => 'string',
      'default' => '',
    );

    $options['accessibility'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['adaptiveHeight'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['autoplay'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['autoplaySpeed'] = array(
      'cast' => 'int',
      'default' => 3000,
    );

    $options['pauseOnHover'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['pauseOnDotsHover'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['arrows'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['appendArrows'] = array(
      'cast' => 'string',
      'default' => '$(element)',
    );

    $options['prevArrow'] = array(
      'cast' => 'string',
      'default' => '<button type="button" data-role="none" class="slick-prev">Previous</button>',
    );

    $options['nextArrow'] = array(
      'cast' => 'string',
      'default' => '<button type="button" data-role="none" class="slick-next">Next</button>',
    );

    $options['centerMode'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['centerPadding'] = array(
      'cast' => 'string',
      'default' => '50px',
    );

    $options['dots'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['dotsClass'] = array(
      'cast' => 'string',
      'default' => 'slick-dots',
    );

    $options['appendDots'] = array(
      'cast' => 'string',
      'default' => '$(element)',
    );

    $options['draggable'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['fade'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['focusOnSelect'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['infinite'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['initialSlide'] = array(
      'cast' => 'int',
      'default' => 0,
    );

    $options['lazyLoad'] = array(
      'cast' => 'string',
      'default' => 'ondemand',
    );

    $options['respondTo'] = array(
      'cast' => 'string',
      'default' => 'window',
    );

    $options['rtl'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['slide'] = array(
      'cast' => 'string',
      'default' => '',
    );

    $options['slidesToShow'] = array(
      'cast' => 'int',
      'default' => 1,
    );

    $options['slidesToScroll'] = array(
      'cast' => 'int',
      'default' => 1,
    );

    $options['speed'] = array(
      'cast' => 'int',
      'default' => 500,
    );

    $options['swipe'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['swipeToSlide'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['edgeFriction'] = array(
      'cast' => 'float',
      'default' => 0.35,
    );

    $options['touchMove'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['touchThreshold'] = array(
      'cast' => 'int',
      'default' => 5,
    );

    $options['useCSS'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['cssEase'] = array(
      'cast' => 'string',
      'default' => 'ease',
    );

    $options['cssEaseBezier'] = array(
      'cast' => 'string',
      'default' => '',
    );

    $options['cssEaseOverride'] = array(
      'cast' => 'string',
      'default' => '',
    );

    $options['easing'] = array(
      'cast' => 'string',
      'default' => 'linear',
    );

    $options['variableWidth'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['vertical'] = array(
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['waitForAnimate'] = array(
      'cast' => 'bool',
      'default' => TRUE,
    );
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
      'breakpoint' => array(
        'cast' => 'int',
        'default' => '',
      ),
      'unslick' => array(
        'cast' => 'bool',
        'default' => FALSE,
      ),
      'settings' => array(),
    );

    // Duplicate main relevant settings.
    foreach ($slick_options as $name => $option) {
      $options[$key]['settings'][$name] = $option;
    }
  }

  return $options;
}

/**
 * Removes problematic options for the responsive Slick.
 *
 * The problematic options are those that should exist once for a given Slick
 *   instance, or no easy way to deal with in the responsive context.
 *   JS takes care of the relevant copy on each responsive setting instead.
 *
 * @param array $options
 *   Array of all available options.
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

  // Allows removable responsive options to be altered.
  drupal_alter('slick_clean_options_info', $cleaned_options);

  return array_diff_key($options, $cleaned_options);
}

/**
 * Default settings for a newly created option set.
 *
 * @param string $key
 *   Gets a specific default value [optional].
 * @param string $set
 *   Gets a specific group of values [optional]: settings, general, responsives.
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 *
 * @return array
 *   An array of default options, or a specific group if $set is specified.
 *
 * @todo simplify the logic by slick_get_default_value().
 */
function _slick_optionset_defaults($key = NULL, $set = NULL, $breakpoints = NULL) {
  $slick_options = slick_get_options();
  $defaults = array();
  foreach ($slick_options as $name => $values) {
    $defaults['settings'][$name] = $values['default'];
  }

  // Breakpoint settings.
  if ($breakpoints) {
    $slick_responsive_options = slick_get_responsive_options($breakpoints);
    foreach ($slick_responsive_options as $i => $items) {
      foreach ($items as $name => $groups) {
        switch ($name) {
          case 'breakpoint':
          case 'unslick':
            $defaults['responsives']['responsive'][$i][$name] = $groups['default'];
            break;

          case 'settings':
            if (is_array($groups)) {
              foreach ($groups as $k => $group) {
                if (is_array($group) && isset($group['default'])) {
                  $defaults['responsives']['responsive'][$i][$name][$k] = $group['default'];
                }
              }
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

  // Return the specific item out of the JS settings alone.
  // @todo drop for slick_get_default_value().
  if (isset($key) && $key && $set && array_key_exists($key, $defaults[$set])) {
    return $defaults[$set][$key];
  }

  // Returns a specific group, or all items.
  return !empty($set) && $defaults[$set] ? $defaults[$set] : $defaults;
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
  $defaults = _slick_optionset_defaults(NULL, $group);
  return isset($defaults[$property]) ? $defaults[$property] : FALSE;
}

/**
 * Strip out options with default values from the Option Set options.
 *
 * @param object $optionset
 *   The Optionset options object.
 * @param array $customs
 *   An array of hand-coded JS options defined using #options property or those
 *   manipulated at theme_slick() dependent on various options.
 *
 * @return array
 *   An array of reduced duplicated Option set options performance wise.
 */
function _slick_remove_default_optionset_options($optionset, $customs = array()) {
  $config = array();

  $options = $optionset->options;
  $breakpoints = $optionset->breakpoints ? $optionset->breakpoints : 0;
  $defaults = _slick_optionset_defaults(NULL, NULL, $breakpoints);

  // Clean up the main options.
  $settings = $customs ? $customs : $options['settings'];
  $config = array_diff_assoc($settings, $defaults['settings']);

  // Must be empty if all is default optionset.
  if ($config) {
    // Remove wasted dependent options if disabled, empty or not.
    if (empty($config['autoplay'])) {
      unset($config['pauseOnHover'], $config['pauseOnDotsHover'], $config['autoplaySpeed']);
    }
    if (empty($config['centerMode'])) {
      unset($config['centerPadding']);
    }
    if (isset($config['arrows']) && !$config['arrows']) {
      unset($config['prevArrow'], $config['nextArrow'], $config['appendArrows']);
    }
    if (empty($config['dots'])) {
      unset($config['dotsClass'], $config['appendDots']);
    }
    if (empty($config['swipe'])) {
      unset($config['swipeToSlide']);
    }
    if (isset($config['cssEaseOverride']) && $config['cssEaseOverride']) {
      if (isset($config['cssEaseBezier']) && $config['cssEaseBezier']) {
        $config['cssEase'] = $config['cssEaseBezier'];
      }
      unset($config['cssEaseOverride'], $config['cssEaseBezier']);
    }
  }

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
          $cleaned[$key]['settings'] = array_diff_assoc($cleaned_responsives[$key]['settings'], $defaults['settings']);

          // Remove wasted dependent options if disabled, empty or not.
          if (empty($cleaned[$key]['settings']['autoplay'])) {
            unset($cleaned[$key]['settings']['pauseOnHover'], $cleaned[$key]['settings']['pauseOnDotsHover'], $cleaned[$key]['settings']['autoplaySpeed']);
          }
          if (empty($cleaned[$key]['settings']['centerMode'])) {
            unset($cleaned[$key]['settings']['centerPadding']);
          }
          if (empty($cleaned[$key]['settings']['swipe'])) {
            unset($cleaned[$key]['settings']['swipeToSlide']);
          }
          if (empty($cleaned[$key]['settings']['dots'])) {
            unset($cleaned[$key]['settings']['dotsClass'], $cleaned[$key]['settings']['appendDots']);
          }
          if (isset($cleaned[$key]['settings']['cssEaseOverride']) && $cleaned[$key]['settings']['cssEaseOverride']) {
            if (isset($cleaned[$key]['settings']['cssEaseBezier']) && $cleaned[$key]['settings']['cssEaseBezier']) {
              $cleaned[$key]['settings']['cssEase'] = $cleaned[$key]['settings']['cssEaseBezier'];
            }
            unset($cleaned[$key]['settings']['cssEaseOverride'], $cleaned[$key]['settings']['cssEaseBezier']);
          }
        }
      }

      // Be sure to follow JS array.
      $config['responsive'] = $cleaned;
    }
  }

  return $config;
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
function _slick_typecast_optionset(&$options = array(), $breakpoints = NULL) {
  if (isset($options) && !empty($options)) {
    $slick_options = slick_get_options();
    foreach ($slick_options as $name => $values) {
      if (isset($slick_options[$name]['cast'])) {
        settype($options['settings'][$name], $slick_options[$name]['cast']);
      }
    }

    if (isset($options['responsives']['responsive']) && $breakpoints) {
      $slick_responsive_options = slick_get_responsive_options($breakpoints);
      foreach ($slick_responsive_options as $i => $items) {
        foreach ($items as $name => $groups) {
          switch ($name) {
            case 'breakpoint':
            case 'unslick':
              settype($options['responsives']['responsive'][$i][$name], $groups['cast']);
              break;

            case 'settings':
              if (is_array($groups)) {
                foreach ($groups as $key => $group) {
                  if (is_array($group) && isset($group['cast']) && isset($group['default'])) {
                    settype($options['responsives']['responsive'][$i][$name][$key], $group['cast']);
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
  }
}

/**
 * Gets supported Slick skins, and additional ones.
 *
 * @param bool $options
 *   Flag to limit the returned values for select options.
 *
 * @return array
 *   An array of available skins.
 */
function slick_skins($options = FALSE) {
  $skins = &drupal_static(__FUNCTION__, NULL);

  if (!isset($skins)) {
    // Collects a list of the currently available skins.
    $skins = module_invoke_all('slick_skins_info');

    // Allows skin information to be altered.
    drupal_alter('slick_skins_info', $skins);

    if ($options) {
      $available_skins = array();
      foreach ($skins as $skin => $properties) {
        $available_skins[$skin] = $properties['name'];
      }
      $skins = $available_skins;
    }
  }

  return $skins;
}

/**
 * Gets supported Slick skins expecting inline CSS background, not images.
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
 * @see slick_optionset_save()
 * @see slick_slick_default_presets()
 */
function slick_optionset_create($values = array()) {
  ctools_include('export');
  $optionset = ctools_export_crud_new('slick_optionset');

  // Set the options to an array.
  $optionset->options = array();

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

  // Merge default settings with any given settings, and removed duplicates.
  $optionset_defaults = _slick_optionset_defaults(NULL, NULL, $optionset->breakpoints);
  $optionset->options = $optionset->options + $optionset_defaults;

  // Clean up default settings.
  if (isset($values['options']['settings']) and is_array($values['options']['settings'])) {
    $optionset->options['settings'] = _slick_remove_default_optionset_options($optionset, $values['options']['settings']);
  }

  return $optionset;
}

/**
 * Fetches all option sets from the database.
 *
 * @return array
 *   The associative array of all options.
 */
function slick_optionset_load_all() {
  ctools_include('export');
  $optionsets = ctools_export_crud_load_all('slick_optionset');
  foreach ($optionsets as $optionset) {
    // Ensure the optionset is typecast after being loaded from DB.
    _slick_typecast_optionset($optionset->options, $optionset->breakpoints);
  }

  return $optionsets;
}

/**
 * Fetches the given option set.
 *
 * @return object
 *   Returns it as an object or NULL, if no set could be found.
 */
function slick_optionset_load($optionset_name) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $optionset_name);
  // Ensure the optionset is typecast after being loaded from DB.
  _slick_typecast_optionset($optionset->options, $optionset->breakpoints);
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
