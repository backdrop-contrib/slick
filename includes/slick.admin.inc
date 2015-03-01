<?php

/**
 * @file
 * Slick admin.
 *
 * Contains optional functions called only if needed by admin pages.
 */

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
 * Checks whether an option set with the given name already exists.
 */
function slick_optionset_exists($optionset_name) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $optionset_name);
  return isset($optionset->name);
}

/**
 * Saves the given option set to the database.
 *
 * Set the $new flag if this set has not been written before.
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

  $optionset_defaults = _slick_optionset_defaults(NULL, $breakpoints);
  $optionset->options = $optionset_defaults += $optionset->options;

  // Prepare the database values.
  $db_values = array(
    'name' => $optionset->name,
    'label' => $optionset->label,
    'breakpoints' => $breakpoints,
    'options' => _slick_typecast_optionset($optionset->options, $breakpoints),
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
 *   Optionset object, or machine name.
 */
function slick_optionset_delete($optionset) {
  if (isset($optionset->name)) {
    $name = $optionset->name;
  }
  else {
    $name = $optionset;
  }
  db_delete('slick_optionset')->condition('name', $name)->execute();
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
