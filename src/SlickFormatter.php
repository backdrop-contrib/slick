<?php

/**
 * @file
 * Contains \Drupal\slick\SlickFormatter.
 */

namespace Drupal\slick;

use Drupal\slick\Entity\Slick;
use Drupal\blazy\BlazyFormatterManager;

/**
 * Implements SlickFormatterInterface.
 */
class SlickFormatter extends BlazyFormatterManager implements SlickFormatterInterface {

  /**
   * {@inheritdoc}
   */
  public function buildSettings(array &$build = [], $items) {
    $settings = &$build['settings'];
    
    // Prepare integration with Blazy.
    $settings['item_id']                  = 'slide';
    $settings['namespace']                = 'slide';
    $settings['theme_hook_image']         = 'slick_image';
    $settings['theme_hook_image_wrapper'] = 'slick_media';
    
    parent::buildSettings($build, $items);

    $optionset_name             = $settings['optionset'] ?: 'default';
    $build['optionset']         = Slick::load($optionset_name);
    $settings['nav']            = !empty($settings['optionset_thumbnail']) && isset($items[1]);
    $settings['lazy']           = empty($settings['responsive_image_style_id']) ? $build['optionset']->getSetting('lazyLoad') : FALSE;
    $settings['blazy']          = $settings['lazy'] == 'blazy' || !empty($settings['blazy']);
    $settings['lazy']           = $settings['blazy'] ? 'blazy' : $settings['lazy'];
    $settings['lazy_attribute'] = $settings['blazy'] ? 'src' : 'lazy';
  }

  /**
   * Gets the thumbnail image.
   */
  public function getThumbnail($settings = []) {
    if (empty($settings['uri'])) {
      return [];
    }
    $thumbnail = [
      '#theme'      => 'image_style',
      '#style_name' => $settings['thumbnail_style'],
      '#uri'        => $settings['uri'],
    ];

    foreach (['height', 'width', 'alt', 'title'] as $data) {
      $thumbnail["#$data"] = isset($settings[$data]) ? $settings[$data] : NULL;
    }
    return $thumbnail;
  }

  /**
   * Gets the media switch options.
   */
  public function getMediaSwitch(array &$element = [], $settings = []) {
    parent::getMediaSwitch($element, $settings);
    $switch = $settings['media_switch'];

    if (isset($element['#url_attributes'])) {
      $element['#url_attributes']['class'] = ['slick__' . $switch, 'litebox'];
    }
  }

}
