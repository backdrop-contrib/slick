<?php

/**
 * @file
 * Contains \Drupal\slick\SlickDefault.
 */

namespace Drupal\slick;

/**
 * Defines shared plugin default settings for field formatter and Views style.
 *
 * @see FormatterBase::defaultSettings()
 * @see StylePluginBase::defineOptions()
 */
class SlickDefault {

  /**
   * Returns basic plugin settings.
   */
  public static function baseSettings() {
    return [
      'cache'               => -1,
      'current_view_mode'   => '',
      'display'             => 'main',
      'optionset'           => 'default',
      'optionset_thumbnail' => '',
      'override'            => FALSE,
      'overridables'        => [],
      'preloader'           => FALSE,
      'skin'                => '',
      'skin_arrows'         => '',
      'skin_dots'           => '',
      'skin_thumbnail'      => '',
      'thumbnail_caption'   => '',
    ];
  }

  /**
   * Returns image-related field formatter and Views settings.
   */
  public static function imageSettings() {
    return [
      'background'          => FALSE,
      'box_style'              => '',
      'caption'                => [],
      'image_style'            => '',
      'layout'                 => '',
      'media_switch'           => '',
      'ratio'                  => '',
      'responsive_image_style' => '',
      'thumbnail_style'        => '',
      'thumbnail_hover'        => FALSE,
      'vanilla'                => FALSE,
    ] + self::baseSettings();
  }

  /**
   * Returns fieldable entity formatter and Views settings.
   */
  public static function extendedSettings() {
    return [
      'class'          => '',
      'dimension'      => '',
      'grid'           => '',
      'grid_medium'    => '',
      'grid_small'     => '',
      'iframe_lazy'    => FALSE,
      'image'          => '',
      'link'           => '',
      'overlay'        => '',
      'preserve_keys'  => FALSE,
      'title'          => '',
      'view_mode'      => '',
      'visible_slides' => '',
    ] + self::imageSettings();
  }

}
