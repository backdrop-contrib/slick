<?php

/**
 * @file
 * Contains \Drupal\slick\SlickDefault.
 */

namespace Drupal\slick;

use Drupal\blazy\Dejavu\BlazyDefault;

/**
 * Defines shared plugin default settings for field formatter and Views style.
 *
 * @see FormatterBase::defaultSettings()
 * @see StylePluginBase::defineOptions()
 *
 * @todo drop: thumbnail_hover, visible_slides.
 */
class SlickDefault extends BlazyDefault {

  /**
   * Returns basic plugin settings.
   */
  public static function baseSettings() {
    return [
      'display'             => 'main',
      'optionset_thumbnail' => '',
      'override'            => FALSE,
      'overridables'        => [],
      'preloader'           => FALSE,
      'skin_arrows'         => '',
      'skin_dots'           => '',
      'skin_thumbnail'      => '',
      'thumbnail_caption'   => '',
    ] + parent::baseSettings();
  }

  /**
   * Returns image-related field formatter and Views settings.
   */
  public static function imageSettings() {
    return [
      'thumbnail_effect' => '',
      'thumbnail_hover'  => FALSE,
    ] + self::baseSettings() + parent::imageSettings();
  }

  /**
   * Returns fieldable entity formatter and Views settings.
   */
  public static function extendedSettings() {
    return [
      'grid'           => 0,
      'grid_header'    => '',
      'grid_medium'    => 0,
      'grid_small'     => 0,
      'preserve_keys'  => FALSE,
      'thumbnail'      => '',
      'visible_items'  => 0,
      'visible_slides' => '',
    ] + self::imageSettings() + parent::extendedSettings();
  }

}
