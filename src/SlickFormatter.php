<?php

namespace Drupal\slick;

use Drupal\slick\Entity\Slick;
use Drupal\blazy\BlazyFormatter;

/**
 * Implements SlickFormatterInterface.
 */
class SlickFormatter extends BlazyFormatter implements SlickFormatterInterface {

  /**
   * {@inheritdoc}
   */
  public function buildSettings(array &$build, $items, $entity) {
    $settings = &$build['settings'];

    // Prepare integration with Blazy.
    $settings['item_id']   = 'slide';
    $settings['namespace'] = 'slick';

    // Pass basic info to parent::buildSettings().
    parent::buildSettings($build, $items, $entity);

    // Load the optionset to work with.
    $optionset = Slick::load($settings['optionset']);

    // Ensures deleted optionset while being used doesn't screw up.
    if (empty($optionset)) {
      $optionset = Slick::load('default');
    }

    // Defines settings which should reach container and down to each item here.
    $settings['nav'] = !empty($settings['optionset_thumbnail']) && isset($items[1]);

    // Do not bother for SlickTextFormatter, or when vanilla is on.
    if (empty($settings['vanilla'])) {
      $lazy              = $optionset->getSetting('lazyLoad');
      $settings['blazy'] = $lazy == 'blazy' || !empty($settings['blazy']);
      $settings['lazy']  = $settings['blazy'] ? 'blazy' : $lazy;

      if (empty($settings['blazy'])) {
        $settings['lazy_class'] = $settings['lazy_attribute'] = 'lazy';
      }
    }
    else {
      // Nothing to work with Vanilla on, disable the asnavfor, else JS error.
      $settings['nav'] = FALSE;
    }

    // Only trim overridables options if disabled.
    if (empty($settings['override']) && isset($settings['overridables'])) {
      $settings['overridables'] = array_filter($settings['overridables']);
    }

    $build['optionset'] = $optionset;

    drupal_alter('slick_settings', $build, $items);

    // Done at top level works, prevents leaking to child for few settings.
    unset($settings['first_item']);
  }

  /**
   * {@inheritdoc}
   */
  public function getThumbnail(array $settings = [], $item = NULL) {
    $thumbnail = [];
    $uri = empty($settings['thumbnail_uri']) ? $settings['uri'] : $settings['thumbnail_uri'];

    if (!empty($uri)) {
      $thumbnail = [
        '#theme'      => 'image_style',
        '#style_name' => $settings['thumbnail_style'] ?: 'thumbnail',
        '#path'       => $uri,
      ];

      // Extract relevant variables from image or file entity/ media.
      foreach (['attributes', 'height', 'weight', 'alt', 'title'] as $key) {
        // Do not output empty value to prevent ugly title undefined.
        if ($item && isset($item->{$key})) {
          $thumbnail["#$key"] = $item->{$key};
        }
      }
    }
    return $thumbnail;
  }

}
