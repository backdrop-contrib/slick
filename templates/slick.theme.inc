<?php

/**
 * @file
 * Hooks and preprocess functions for slick module.
 */

use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\Xss;
use Drupal\Component\Serialization\Json;
use Drupal\Core\Template\Attribute;
use Drupal\Core\Render\Element;

/**
 * Prepares variables for slick templates.
 *
 * Default template: slick.html.twig.
 *
 * @param array $variables
 *   Elements:
 *   - #settings is set via sub-modules and serves various purposes, and not
 *   related to JS settings, mostly slide layouts or attaching assets.
 *   - #options is set programmatically, or hand-crafted, and only accepts
 *   direct key|value pairs related to JS settings, or at least optionset name.
 */
function template_preprocess_slick(array &$variables) {
  $element = $variables['elements'];
  $count = count(Element::children($element));
  $options = (isset($element['#options'])) ? $element['#options'] : array();

  // Set the ID for each slick instance, if none provided, for quick JS lookup.
  if (empty($variables['attributes']['id'])) {
    $id = (isset($element['#id'])) ? Html::getUniqueId($element['#id']) : Html::getUniqueId('slick');
    $variables['attributes']['id'] = $id;
  }

  foreach (Element::children($element) as $key) {
    $child = &$element[$key];
    // Default children #theme to slick_item if nothing is set.
    if (!isset($child['#theme_wrappers'])) {
      $child['#theme_wrappers'] = array('slick_slide');
    }
    if (!isset($child['#delta'])) {
      $child['#delta'] = $key;
    }
    $variables['content'][$key] = $child;
  }

  $variables['options'] = $options;
  if (!empty($options)) {
    $js = Json::encode($variables['options']);
    $variables['attributes']['data-slick'] = $js;
  }

  // Add basic library.
  if ($count > 1) {
    $variables['#attached']['library'][] = 'slick/slick.load';
  }
}

/**
 * Prepares variables for slick item templates.
 *
 * Default template: slick-slide.html.twig.
 *
 * @param array $variables
 *   Element:
 *   - #settings is set via sub-modules and serves various purposes, and not
 *   related to JS settings, mostly slide layouts or attaching assets.
 *   - #options is set programmatically, or hand-crafted, and only accepts
 *   direct key|value pairs related to JS settings, or at least optionset name.
 *   - #delta is key of the array entry used to generate a unique css class.
 */
function template_preprocess_slick_slide(array &$variables) {
  $element = $variables['element'];
  $options = $element['#options'] = array();
  $variables['delta'] = $element['#delta'];

  $variables['tag'] = $variables['content_tag'] = 'div';
  $variables['content_attributes'] = new Attribute();

  $variables['css_name'] = Html::cleanCssIdentifier(implode('-', array('slick-slide', $variables['delta'])));

  $variables['children'] = $element['#children'];

  $variables['options'] = $options;
}
