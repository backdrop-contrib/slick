<?php
/**
 * @file
 * Contains optional functions called only if needed by admin pages.
 */

/**
 * Defines a list of options available for the responsive Slick.
 *
 * @param int $count
 *   The number of breakpoints.
 *
 * @return array
 *   A copy of the main options with the removal of problematic options.
 */
function slick_get_responsive_options($count = 0) {
  $options = array();
  $breakpoints = drupal_map_assoc(range(0, ($count - 1)));
  $defaults = slick_get_options();

  foreach ($breakpoints as $key => $breakpoint) {
    $options[$key] = array(
      'breakpoint' => 0,
      'unslick' => FALSE,
      'settings' => array(),
    );

    foreach (slick_clean_options($defaults) as $name => $value) {
      $options[$key]['settings'][$name] = $value;
    }
  }

  return $options;
}

/**
 * Removes problematic options for the responsive Slick.
 *
 * They should exist once for a given Slick, or not easy to deal with, or make
 *   no sense in the responsive context. JS takes care of their relevant copy.
 *
 * @param array $options
 *   An array of all available options, can be form elements, or basic ones.
 *
 * @return array
 *   An array of cleaned out options for the responsive displays.
 */
function slick_clean_options(array $options) {
  $excludes = drupal_map_assoc(array(
    'mobileFirst',
    'asNavFor',
    'slide',
    'lazyLoad',
    'prevArrow',
    'nextArrow',
    'appendArrows',
    'dotsClass',
    'appendDots',
    'rtl',
  ));

  drupal_alter('slick_clean_options_info', $excludes);
  return array_diff_key($options, $excludes);
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

  drupal_alter('slick_overridable_options_info', $overridables);
  return $overridables;
}

/**
 * Gets default layout options for the core Image, or Views.
 *
 * @return array
 *   An array of available basic layouts.
 */
function slick_layouts() {
  $layouts = &drupal_static(__FUNCTION__, NULL);

  if (!isset($layouts)) {
    $layouts = array(
      'bottom'      => t('Caption bottom'),
      'top'         => t('Caption top'),
      'right'       => t('Caption right'),
      'left'        => t('Caption left'),
      'center'      => t('Caption center'),
      'center-top'  => t('Caption center top'),
      'below'       => t('Caption below the slide'),
      'stage-right' => t('Caption left, stage right'),
      'stage-left'  => t('Caption right, stage left'),
      'split-right' => t('Caption left, stage right, split half'),
      'split-left'  => t('Caption right, stage left, split half'),
      'stage-zebra' => t('Stage zebra'),
      'split-zebra' => t('Split half zebra'),
    );
  }

  return $layouts;
}

/**
 * Gets supported Slick skins, and additional ones for select options.
 *
 * @return array
 *   An array of available skins.
 */
function slick_skins_options() {
  $skins = &drupal_static(__FUNCTION__, NULL);

  if (!isset($skins)) {
    $skins = array();
    foreach (slick_skins() as $skin => $properties) {
      $skins[$skin] = $properties['name'];
    }
  }

  return $skins;
}

/**
 * Returns supported Slick skin dots for select options.
 *
 * @return array
 *   The associative array of available skin dots.
 */
function slick_dots_options() {
  $dots = &drupal_static(__FUNCTION__, NULL);

  if (!isset($dots)) {
    $dots = array();
    foreach (slick_dots() as $key => $properties) {
      $dots[$key] = $properties['name'];
    }
  }

  return $dots;
}

/**
 * Returns supported Slick skin arrows for select options.
 *
 * @return array
 *   The associative array of available skin arrows.
 */
function slick_arrows_options() {
  $arrows = &drupal_static(__FUNCTION__, NULL);

  if (!isset($arrows)) {
    $arrows = array();
    foreach (slick_arrows() as $key => $properties) {
      $arrows[$key] = $properties['name'];
    }
  }

  return $arrows;
}

/**
 * Load all slick optionsets as select options.
 *
 * @return array
 *   An array containing slicks indexed by their names.
 */
function slick_optionset_options() {
  $optionsets = &drupal_static(__FUNCTION__, NULL);

  if (!isset($optionsets)) {
    $optionsets = array();
    $slicks = slick_optionset_load_all();
    foreach ($slicks as $key => $optionset) {
      $optionsets[$key] = check_plain($optionset->label);
    }
    asort($optionsets);
  }

  return $optionsets;
}

/**
 * Checks whether an optionset with the given name already exists.
 *
 * @param string $optionset_name
 *   The Optionset machine name.
 *
 * @return bool
 *   Returns TRUE if exists, FALSE otherwise.
 */
function slick_optionset_exists($optionset_name) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $optionset_name);
  return isset($optionset->name);
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
 * Saves the given option set to the database.
 *
 * Set the $new flag if this set has not been written before.
 *
 * @param object $optionset
 *   The Optionset object.
 * @param bool $new
 *   The Optionset machine name.
 *
 * @return object
 *   Returns the newly saved object, FALSE otherwise.
 */
function slick_optionset_save($optionset, $new = FALSE) {
  // If the machine name is missing or already in use, return an error.
  if (empty($optionset->name) or (FALSE != slick_optionset_exists($optionset->name) and $new)) {
    return FALSE;
  }

  // Check for an invalid list of options.
  if (isset($optionset->options) and !is_array($optionset->options)) {
    return FALSE;
  }

  if (empty($optionset->label)) {
    $optionset->label = $optionset->name;
  }

  // Merge default settings with any given settings.
  $breakpoints = 0;
  if (isset($optionset->breakpoints)) {
    $breakpoints = $optionset->breakpoints;
  }

  $defaults['general'] = array('goodies' => array());
  $defaults['settings'] = slick_get_options();
  $optionset->options = $optionset->options + $defaults;
  _slick_typecast_optionset($optionset->options, $breakpoints);

  // Prepare the database values.
  $db_values = array(
    'name' => $optionset->name,
    'label' => $optionset->label,
    'breakpoints' => $breakpoints,
    'options' => $optionset->options,
  );

  if ($new) {
    $result = drupal_write_record('slick_optionset', $db_values);
  }
  else {
    $result = drupal_write_record('slick_optionset', $db_values, 'name');
  }

  // Return the object if the values were saved successfully.
  if (($new and SAVED_NEW == $result) or (!$new and SAVED_UPDATED == $result)) {
    return $optionset;
  }

  // Otherwise, an error occured.
  return FALSE;
}

/**
 * Deletes the given option set from the database.
 *
 * @param string|object $optionset
 *   Optionset object, or string machine name.
 */
function slick_optionset_delete($optionset) {
  ctools_include('export');
  $object = is_object($optionset) ? $optionset : slick_optionset_load($optionset);

  // This only deletes from the database, which means that if an item is in
  // code, then this is actually a revert.
  ctools_export_crud_delete('slick_optionset', $object);
}

/**
 * Returns the typecast values, so that JSON object has the right values.
 *
 * @param array $options
 *   An array of Optionset options.
 * @param int $breakpoints
 *   The number of breakpoints if specified.
 */
function _slick_typecast_optionset(&$options = array(), $breakpoints = 0) {
  if (empty($options)) {
    return;
  }

  $slick_options = slick_get_options();
  foreach ($slick_options as $name => $value) {
    if (isset($options['settings'][$name])) {
      $cast = gettype($slick_options[$name]);
      settype($options['settings'][$name], $cast);
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
                $cast = gettype($item[$key]);
                settype($options['responsives']['responsive'][$i][$name][$key], $cast);
              }
            }
            break;

          default:
            break;
        }
      }
    }
  }

  drupal_alter('slick_typecast_optionset_info', $options, $breakpoints);
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
    list(, $css_easing) = array_pad(array_map('trim', explode("|", $easings[$easing], 2)), 2, NULL);
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
      list($readable_easing, $bezier) = array_pad(array_map('trim', explode("|", $easing, 2)), 2, NULL);
      $css_easings[$key] = $all ? $easing : $readable_easing;
      // Satisfy both phpcs and coder since no skip tolerated.
      unset($bezier);
    }
  }

  return $css_easings;
}

/**
 * A helper function to return view modes for a form.
 *
 * @param string $entity_type
 *   The entity type to use with entity_get_info($entity_type) or
 *   entity_get_info($field['settings']['target_type']).
 * @param bool $exclude
 *   If TRUE, then exclude some view modes, such as: 'rss', 'search_index',
 *   'search_result', 'print', 'token'.
 *
 * @return array
 *   An array of available view modes, excluding some.
 */
function slick_get_view_modes($entity_type, $exclude = TRUE) {
  $view_mode_options = &drupal_static(__FUNCTION__);

  if (!isset($view_mode_options)) {
    $view_mode_options = array('default' => t('Default'));
    $view_mode_excludes = array(
      'rss',
      'search_index',
      'search_result',
      'print',
      'token',
      'preview',
      'wysiwyg',
    );

    $entity_info = entity_get_info($entity_type);
    if (!empty($entity_info['view modes'])) {
      foreach ($entity_info['view modes'] as $view_mode => $view_mode_settings) {
        if ($exclude && in_array($view_mode, $view_mode_excludes)) {
          continue;
        }
        $view_mode_options[$view_mode] = $view_mode_settings['label'];
      }
    }
  }

  return $view_mode_options;
}

/**
 * Gets a list of fields in the Field Collection item.
 */
function slick_get_fc_fields($instance, $field_types) {
  $fc_options = array();
  $fields = field_info_fields();
  $bundle = 'field_collection_item';
  $bundle_instance = $instance['field_name'];

  foreach ($fields as $name => $field) {
    $infos = field_info_instance($bundle, $name, $bundle_instance);
    if ($field_types == '_all') {
      $fc_options[$name] = $infos['label'];
    }
    else {
      if (in_array($bundle, array_keys($field['bundles']))
          && in_array($bundle_instance, $field['bundles'][$bundle])
          && in_array($field['type'], $field_types)) {
        $fc_options[$name] = $infos['label'];
      }
    }
  }

  return $fc_options;
}

/**
 * Gets a list of fields in the Media file field.
 */
function slick_get_media_fields($instance, $widget_types = NULL) {
  $media_options = array();
  $types = drupal_map_assoc(array('audio', 'image', 'video'));

  // Expose all media sub-fields, and merge them as available options.
  foreach ($types as $media_type) {
    $fields = field_info_instances('file', $media_type);
    foreach ($fields as $key => $media_field) {
      $type = $media_field['widget']['type'];
      if ($widget_types && in_array($type, $widget_types)) {
        $media_options[$key] = $media_field['label'];
      }
      else {
        $media_options[$key] = $media_field['label'];
      }
    }
  }

  return $media_options;
}
