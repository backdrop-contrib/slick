<?php

/**
 * @file
 * Slick elements.
 *
 * This file is called by admin and several front end functions.
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
      'title' => t('Mobile first'),
      'description' => t('Responsive settings use mobile first calculation.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['asNavFor'] = array(
      'title' => t('asNavFor target'),
      'description' => t('Set the slider to be the navigation of other slider (Class or ID Name). Use selector identifier ("." or "#") accordingly. If class, use the provided Wrapper class under General as needed, e.g.: if the main display has class "slick--for", and the thumbnail navigation "slick--nav", place the opposite here as its target. Or use existing classes based on optionsets, e.g.: .slick--optionset--main, or .slick--optionset--main-nav. Overriden per field formatter.'),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '',
    );

    $options['accessibility'] = array(
      'title' => t('Accessibility'),
      'description' => t('Enables tabbing and arrow key navigation.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['adaptiveHeight'] = array(
      'title' => t('Adaptive height'),
      'description' => t('Enables adaptive height for single slide horizontal carousels.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['autoplay'] = array(
      'title' => t('Autoplay'),
      'description' => t('Enables autoplay.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['autoplaySpeed'] = array(
      'title' => t('Autoplay speed'),
      'description' => t('Autoplay speed in milliseconds.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 3000,
      'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
    );

    $options['pauseOnHover'] = array(
      'title' => t('Pause on hover'),
      'description' => t('Pause autoplay on hover.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
      'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
    );

    $options['pauseOnDotsHover'] = array(
      'title' => t('Pause on dots hover'),
      'description' => t('Pauses autoplay when a dot is hovered.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
      'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
    );

    $options['arrows'] = array(
      'title' => t('Arrows'),
      'description' => t('Show prev/next arrows'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['appendArrows'] = array(
      'title' => t('Append arrows'),
      'description' => t("Change where the navigation arrows are attached (Selector, htmlString, Array, Element, jQuery object). Leave it to default to wrap it within .slick__arrow container, otherwise change its markups accordingly."),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '$(element)',
      'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
    );

    $options['prevArrow'] = array(
      'title' => t('Previous arrow'),
      'description' => t("Customize the previous arrow markups. Make sure to keep the expected class."),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '<button type="button" data-role="none" class="slick-prev">Previous</button>',
      'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
    );

    $options['nextArrow'] = array(
      'title' => t('Next arrow'),
      'description' => t("Customize the next arrow markups. Make sure to keep the expected class."),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '<button type="button" data-role="none" class="slick-next">Next</button>',
      'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
    );

    $options['centerMode'] = array(
      'title' => t('Center mode'),
      'description' => t('Enables centered view with partial prev/next slides. Use with odd numbered slidesToShow counts.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['centerPadding'] = array(
      'title' => t('Center padding'),
      'description' => t('Side padding when in center mode (px or %). Be aware, too large padding at small breakpoint will screw the slide calculation with slidesToShow.'),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '50px',
      'states' => array('visible' => array(':input[name*="options[settings][centerMode]"]' => array('checked' => TRUE))),
    );

    $options['dots'] = array(
      'title' => t('Dots'),
      'description' => t('Show dot indicators.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['dotsClass'] = array(
      'title' => t('Dot class'),
      'description' => t('Class for slide indicator dots container. Do not prefix with dot. If you change this, edit its CSS accordingly.'),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => 'slick-dots',
      'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
    );

    $options['appendDots'] = array(
      'title' => t('Append dots'),
      'description' => t('Change where the navigation dots are attached (Selector, htmlString, Array, Element, jQuery object). If you change this, make sure to provide its relevant markup.'),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '$(element)',
      'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
    );

    $options['draggable'] = array(
      'title' => t('Draggable'),
      'description' => t('Enable mouse dragging.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['fade'] = array(
      'title' => t('Fade'),
      'description' => t('Enable fade'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['focusOnSelect'] = array(
      'title' => t('Focus on select'),
      'description' => t('Enable focus on selected element (click).'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['infinite'] = array(
      'title' => t('Infinite'),
      'description' => t('Infinite loop sliding.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['initialSlide'] = array(
      'title' => t('Initial slide'),
      'description' => t('Slide to start on.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 0,
    );

    $options['lazyLoad'] = array(
      'title' => t('Lazy load'),
      'description' => t("Set lazy loading technique. 'ondemand' will load the image as soon as you slide to it, 'progressive' loads one image after the other when the page loads. Note: dummy image is no good for ondemand. If ondemand fails to generate images, try progressive instead. Or use <a href='@url' target='_blank'>imageinfo_cache</a>. To share images for Pinterest, leave empty, otherwise no way to read actual image src.", array('@url' => '//www.drupal.org/project/imageinfo_cache')),
      'type' => 'select',
      'cast' => 'string',
      'default' => 'ondemand',
      'options' => drupal_map_assoc(array('ondemand', 'progressive')),
      'empty_option' => t('- None -'),
    );

    $options['respondTo'] = array(
      'title' => t('Respond to'),
      'description' => t("Width that responsive object responds to. Can be 'window', 'slider' or 'min' (the smaller of the two)."),
      'type' => 'select',
      'cast' => 'string',
      'default' => 'window',
      'options' => drupal_map_assoc(array('window', 'slider', 'min')),
    );

    $options['rtl'] = array(
      'title' => t('RTL'),
      'description' => t("Change the slider's direction to become right-to-left."),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['slide'] = array(
      'title' => t('Slide element'),
      'description' => t("Element query to use as slide. Slick will use any direct children as slides, without having to specify which tag or selector to target."),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => '',
    );

    $options['slidesToShow'] = array(
      'title' => t('Slides to show'),
      'description' => t('Number of slides to show at a time. If 1, it will behave like slideshow, more than 1 a carousel. Provide more if it is a thumbnail navigation with asNavFor. Only works with odd number slidesToShow counts when using centerMode.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 1,
    );

    $options['slidesToScroll'] = array(
      'title' => t('Slides to scroll'),
      'description' => t('Number of slides to scroll at a time, or steps at each scroll.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 1,
    );

    $options['speed'] = array(
      'title' => t('Speed'),
      'description' => t('Slide/Fade animation speed in milliseconds.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 500,
      'field_suffix' => 'ms',
    );

    $options['swipe'] = array(
      'title' => t('Swipe'),
      'description' => t('Enable swiping.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['swipeToSlide'] = array(
      'title' => t('Swipe to slide'),
      'description' => t('Allow users to drag or swipe directly to a slide irrespective of slidesToScroll.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
      'states' => array('visible' => array(':input[name*="options[settings][swipe]"]' => array('checked' => TRUE))),
    );

    $options['edgeFriction'] = array(
      'title' => t('Edge friction'),
      'description' => t("Resistance when swiping edges of non-infinite carousels. If you don't want resistance, set it to 1."),
      'type' => 'textfield',
      'cast' => 'float',
      'default' => 0.35,
    );

    $options['touchMove'] = array(
      'title' => t('Touch move'),
      'description' => t('Enable slide motion with touch.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['touchThreshold'] = array(
      'title' => t('Touch threshold'),
      'description' => t('Swipe distance threshold.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => 5,
      'states' => array('visible' => array(':input[name*="options[settings][touchMove]"]' => array('checked' => TRUE))),
    );

    $options['useCSS'] = array(
      'title' => t('Use CSS'),
      'description' => t('Enable/Disable CSS Transitions.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => TRUE,
    );

    $options['cssEase'] = array(
      'title' => t('CSS ease'),
      'description' => t('CSS3 animation easing. <a href="@ceaser">Learn</a> <a href="@bezier">more</a>.', array('@ceaser' => '//matthewlein.com/ceaser/', '@bezier' => '//cubic-bezier.com')),
      'type' => 'textfield',
      'cast' => 'string',
      'default' => 'ease',
      'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
    );

    $options['cssEaseOverride'] = array(
      'title' => t('CSS ease override'),
      'description' => t('If provided, this will override the CSS ease with the pre-defined CSS easings based on <a href="@ceaser">CSS Easing Animation Tool</a>. Leave it empty to use your own CSS ease.', array('@ceaser' => '//matthewlein.com/ceaser/')),
      'type' => 'select',
      'cast' => 'string',
      'default' => '',
      'options' => _slick_css_easing_options(),
      'empty_option' => t('- None -'),
      'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
    );

    $options['easing'] = array(
      'title' => t('jQuery easing'),
      'description' => t('Add easing for jQuery animate as fallback. Use with <a href="@easing">easing</a> libraries or default easing methods. Optionally install <a href="@jqeasing">jqeasing module</a>. This will be ignored and replaced by CSS ease for supporting browsers, or effective if useCSS is disabled.', array('@jqeasing' => '//drupal.org/project/jqeasing', '@easing' => '//gsgd.co.uk/sandbox/jquery/easing/')),
      'type' => 'select',
      'cast' => 'string',
      'default' => 'linear',
      'options' => _slick_easing_options(),
      'empty_option' => t('- None -'),
    );

    $options['variableWidth'] = array(
      'title' => t('variableWidth'),
      'description' => t('Disables automatic slide width calculation.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['vertical'] = array(
      'title' => t('Vertical'),
      'description' => t('Vertical slide direction.'),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options['waitForAnimate'] = array(
      'title' => t('waitForAnimate'),
      'description' => t('Ignores requests to advance the slide while animating.'),
      'type' => 'checkbox',
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

  foreach ($breakpoints as $key => $breakpoint) {
    $options[$key] = array(
      'title' => t('Breakpoint #@key', array('@key' => ($key + 1))),
      'type' => 'fieldset',
    );

    $options[$key]['breakpoint'] = array(
      'title' => t('Breakpoint'),
      'description' => t('Breakpoint width in pixel.'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => '',
      'field_suffix' => 'px',
    );

    $options[$key]['unslick'] = array(
      'title' => t('Unslick'),
      'description' => t("Disable Slick at a given breakpoint. Note, you can't window shrink this, once you unslick, you are unslicked."),
      'type' => 'checkbox',
      'cast' => 'bool',
      'default' => FALSE,
    );

    $options[$key]['settings'] = array();

    // Duplicate main settings.
    // Clean up problematic dup options, since they have to be available once
    // for all. JS takes care of the copy on each responsive settings.
    unset(
      $slick_options['mobileFirst'],
      $slick_options['asNavFor'],
      $slick_options['slide'],
      $slick_options['lazyLoad'],
      $slick_options['prevArrow'],
      $slick_options['nextArrow'],
      $slick_options['appendArrows'],
      $slick_options['dotsClass'],
      $slick_options['appendDots'],
      $slick_options['rtl']
    );
    foreach ($slick_options as $name => $values) {
      $options[$key]['settings'][$name] = $values;
    }
  }

  return $options;
}

/**
 * Default settings for a newly created option set.
 *
 * @param string $key
 *   Gets a specific default value [optional].
 *
 * @param string $set
 *   Gets a specific group of values [optional]: settings, general, responsives.
 *
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 *
 * @return array
 *   An array of default options, or a specific group if $set is specified.
 */
function _slick_optionset_defaults($key = NULL, $set = NULL, $breakpoints = NULL) {

  // Main slick settings.
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
  if (isset($key) && $key && $set && array_key_exists($key, $defaults[$set])) {
    return $defaults[$set][$key];
  }

  // Returns a specific group, or all items.
  return !empty($set) && $defaults[$set] ? $defaults[$set] : $defaults;
}

/**
 * Strip out options with default values from the Option Set options.
 *
 * @param object $optionset
 *   An array of Option Set options, default is loaded if none specified.
 *
 * @param array $customs
 *   An array of hand-coded JS options defined using #options property or those
 *   manupulated at theme_slick() dependent on various options.
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
    // Remove wasted dependent options if disabled, even if not empty.
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
      $config['cssEase'] = _slick_css_easing_mapping($config['cssEaseOverride']);
      unset($config['cssEaseOverride']);
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

          // Remove wasted dependent options if disabled, even if not empty.
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
            $cleaned[$key]['settings']['cssEase'] = _slick_css_easing_mapping($cleaned[$key]['settings']['cssEaseOverride']);
            unset($cleaned[$key]['settings']['cssEaseOverride']);
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
 * Adds the typecasting to the given values.
 *
 * With this, the generated JSON object has the right values.
 *
 * @param array $options
 *   An array of Option Set options, default is loaded if none specified.
 *
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 *
 * @return array
 *   An array of valid typecast values ready to consume by jQuery.
 */
function _slick_typecast_optionset(&$options, $breakpoints = NULL) {
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
 * List of all easing methods available from jQuery Easing v1.3.
 *
 * @return array
 *   An array of available jQuery Easing options as fallback for browsers that
 *   don't support pure CSS easing methods.
 */
function _slick_easing_options() {
  $easings = &drupal_static(__FUNCTION__, NULL);

  if (!isset($easings)) {
    $easings = array(
      'linear' => 'Linear',
      'swing' => 'Swing',
      'easeInQuad' => 'easeInQuad',
      'easeOutQuad' => 'easeOutQuad',
      'easeInOutQuad' => 'easeInOutQuad',
      'easeInCubic' => 'easeInCubic',
      'easeOutCubic' => 'easeOutCubic',
      'easeInOutCubic' => 'easeInOutCubic',
      'easeInQuart' => 'easeInQuart',
      'easeOutQuart' => 'easeOutQuart',
      'easeInOutQuart' => 'easeInOutQuart',
      'easeInQuint' => 'easeInQuint',
      'easeOutQuint' => 'easeOutQuint',
      'easeInOutQuint' => 'easeInOutQuint',
      'easeInSine' => 'easeInSine',
      'easeOutSine' => 'easeOutSine',
      'easeInOutSine' => 'easeInOutSine',
      'easeInExpo' => 'easeInExpo',
      'easeOutExpo' => 'easeOutExpo',
      'easeInOutExpo' => 'easeInOutExpo',
      'easeInCirc' => 'easeInCirc',
      'easeOutCirc' => 'easeOutCirc',
      'easeInOutCirc' => 'easeInOutCirc',
      'easeInElastic' => 'easeInElastic',
      'easeOutElastic' => 'easeOutElastic',
      'easeInOutElastic' => 'easeInOutElastic',
      'easeInBack' => 'easeInBack',
      'easeOutBack' => 'easeOutBack',
      'easeInOutBack' => 'easeInOutBack',
      'easeInBounce' => 'easeInBounce',
      'easeOutBounce' => 'easeOutBounce',
      'easeInOutBounce' => 'easeInOutBounce',
    );
  }
  return $easings;
}

/**
 * Maps existing jQuery easing value to equivalent CSS easing methods.
 *
 * @param string $easing
 *   The name of the human readable easing.
 *
 * @return string
 *   A string of unfriendly bezier equivalent for the Slick, or NULL.
 */
function _slick_css_easing_mapping($easing = NULL) {
  $css_easing = '';

  if ($easing) {
    $easings = _slick_css_easing_options(TRUE);
    list($readable_easing, $bezier) = array_pad(array_map('trim', explode("|", $easings[$easing], 2)), 2, NULL);
    $css_easing = $bezier;
  }

  return $css_easing;
}

/**
 * List of available pure CSS easing methods.
 *
 * @param bool $all
 *   Flag to output the array as is for further processing.
 *
 * @return array
 *   An array of CSS easings for select options, or all for the mappings.
 *
 * @see https://github.com/kenwheeler/slick/issues/118
 * @see http://matthewlein.com/ceaser/
 * @see http://www.w3.org/TR/css3-transitions/
 */
function _slick_css_easing_options($all = FALSE) {
  $css_easings = &drupal_static(__FUNCTION__, NULL);

  if (!isset($css_easings)) {
    $css_easings = array();
    $available_easings = array(

      // Defaults/ Native.
      'ease' => 'ease|ease',
      'linear' => 'linear|linear',
      'ease-in' => 'ease-in|ease-in',
      'ease-out' => 'ease-out|ease-out',
      'swing' => 'swing|ease-in-out',

      // Penner Equations (approximated).
      'easeInQuad' => 'easeInQuad|cubic-bezier(0.550, 0.085, 0.680, 0.530)',
      'easeInCubic' => 'easeInCubic|cubic-bezier(0.550, 0.055, 0.675, 0.190)',
      'easeInQuart' => 'easeInQuart|cubic-bezier(0.895, 0.030, 0.685, 0.220)',
      'easeInQuint' => 'easeInQuint|cubic-bezier(0.755, 0.050, 0.855, 0.060)',
      'easeInSine' => 'easeInSine|cubic-bezier(0.470, 0.000, 0.745, 0.715)',
      'easeInExpo' => 'easeInExpo|cubic-bezier(0.950, 0.050, 0.795, 0.035)',
      'easeInCirc' => 'easeInCirc|cubic-bezier(0.600, 0.040, 0.980, 0.335)',
      'easeInBack' => 'easeInBack|cubic-bezier(0.600, -0.280, 0.735, 0.045)',
      'easeOutQuad' => 'easeOutQuad|cubic-bezier(0.250, 0.460, 0.450, 0.940)',
      'easeOutCubic' => 'easeOutCubic|cubic-bezier(0.215, 0.610, 0.355, 1.000)',
      'easeOutQuart' => 'easeOutQuart|cubic-bezier(0.165, 0.840, 0.440, 1.000)',
      'easeOutQuint' => 'easeOutQuint|cubic-bezier(0.230, 1.000, 0.320, 1.000)',
      'easeOutSine' => 'easeOutSine|cubic-bezier(0.390, 0.575, 0.565, 1.000)',
      'easeOutExpo' => 'easeOutExpo|cubic-bezier(0.190, 1.000, 0.220, 1.000)',
      'easeOutCirc' => 'easeOutCirc|cubic-bezier(0.075, 0.820, 0.165, 1.000)',
      'easeOutBack' => 'easeOutBack|cubic-bezier(0.175, 0.885, 0.320, 1.275)',
      'easeInOutQuad' => 'easeInOutQuad|cubic-bezier(0.455, 0.030, 0.515, 0.955)',
      'easeInOutCubic' => 'easeInOutCubic|cubic-bezier(0.645, 0.045, 0.355, 1.000)',
      'easeInOutQuart' => 'easeInOutQuart|cubic-bezier(0.770, 0.000, 0.175, 1.000)',
      'easeInOutQuint' => 'easeInOutQuint|cubic-bezier(0.860, 0.000, 0.070, 1.000)',
      'easeInOutSine' => 'easeInOutSine|cubic-bezier(0.445, 0.050, 0.550, 0.950)',
      'easeInOutExpo' => 'easeInOutExpo|cubic-bezier(1.000, 0.000, 0.000, 1.000)',
      'easeInOutCirc' => 'easeInOutCirc|cubic-bezier(0.785, 0.135, 0.150, 0.860)',
      'easeInOutBack' => 'easeInOutBack|cubic-bezier(0.680, -0.550, 0.265, 1.550)',
    );

    foreach ($available_easings as $key => $easing) {
      list($readable_easing, $css_easing) = array_pad(array_map('trim', explode("|", $easing, 2)), 2, NULL);
      $css_easings[$key] = $all ? $easing : $readable_easing;
    }
  }

  return $css_easings;
}

/**
 * Create a new optionset object.
 *
 * Note that this function does not save the optionset to the database.
 * @see slick_optionset_save()
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

  $optionset->breakpoints = 0;
  if (isset($values['breakpoints'])) {
    $optionset->breakpoints = $values['breakpoints'];
  }

  if (isset($values['options']) and is_array($values['options'])) {
    $optionset->options = $values['options'];
  }

  // Merge default settings with any given settings.
  $optionset_defaults = _slick_optionset_defaults(NULL, NULL, $optionset->breakpoints);
  $optionset->options = $optionset_defaults += $optionset->options;

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
