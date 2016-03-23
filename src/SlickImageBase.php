<?php

/**
 * @file
 * Contains \Drupal\slick\SlickImageBase.
 */

namespace Drupal\slick;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Cache\Cache;
use Drupal\Component\Serialization\Json;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides base lazyloaded image and thumbnail building.
 *
 * @see \Drupal\slick\Plugin\Field\FieldFormatter\SlickFormatter
 *
 * @todo drop for Blazy to have re-usable unified image formatter managements.
 */
abstract class SlickImageBase {

  /**
   * The slick manager service.
   *
   * @var \Drupal\slick\SlickManagerInterface.
   */
  protected $manager;

  /**
   * Constructs a SlickImageBase object.
   *
   * @param \Drupal\slick\SlickManagerInterface $manager
   *   The slick manager service.
   */
  public function __construct(SlickManagerInterface $manager) {
    $this->manager = $manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('slick.manager'));
  }

  /**
   * Returns the slick service.
   */
  public function manager() {
    return $this->manager;
  }

  /**
   * Gets the image based on the Responsive image mapping, or Slick image lazy.
   *
   * @todo drop for or extend \Drupal\blazy\BlazyManager instead.
   */
  public function getImage($build = []) {
    $item     = $build['item'];
    $settings = &$build['settings'];

    $this->getUrlDimensions($settings, $item, $settings['image_style']);

    $style_tags = isset($settings['style_tags']) ? $settings['style_tags'] : [];
    $file_tags  = isset($settings['file_tags'])  ? $settings['file_tags']  : [];

    $image = [
      '#theme'          => 'slick_image',
      '#item'           => [],
      '#delta'          => $settings['delta'],
      '#build'          => $build,
      '#pre_render'     => [[$this, 'preRenderImage']],
      '#cache'          => ['tags' => Cache::mergeTags($style_tags, $file_tags)],
      '#theme_wrappers' => ['slick_media'],
    ];

    $this->manager->getModuleHandler()->alter('slick_image', $image, $settings);

    // Build the slide with responsive image, lightbox or multimedia supports.
    return $image;
  }

  /**
   * Builds the Slick image as a structured array ready for ::renderer().
   *
   * @todo drop for \Drupal\blazy\BlazyManager instead.
   */
  public function preRenderImage($element) {
    $build = $element['#build'];
    $item  = $build['item'];
    unset($element['#build']);

    if (empty($item)) {
      return [];
    }

    $settings = $build['settings'];
    $element['#item'] = $item;

    // Extract field item attributes for the theme function, and unset them
    // from the $item so that the field template does not re-render them.
    $item_attributes = $item->_attributes;
    unset($item->_attributes);
    $element['#item_attributes'] = $item_attributes;

    // Responsive image integration.
    if (!empty($settings['resimage']) && !empty($settings['responsive_image_style'])) {
      $responsive_image_style = $this->manager->entityLoad($settings['responsive_image_style'], 'responsive_image_style');
      $settings['responsive_image_style_id'] = $responsive_image_style->id() ?: '';
      $element['#cache'] = [
        'tags' => $this->getResponsiveImageCacheTags($responsive_image_style),
      ];
    }
    elseif (!empty($settings['width'])) {
      $element['#item_attributes']['height'] = $settings['height'];
      $element['#item_attributes']['width']  = $settings['width'];
      if (!empty($settings['blazy'])) {
        $settings['lazy_attribute'] = 'src';
        $element['#item_attributes']['class'][] = 'b-lazy';
      }
    }

    if (!empty($settings['thumbnail_style'])) {
      $element['#item_attributes']['data-thumb'] = $this->manager->entityLoad($settings['thumbnail_style'], 'image_style')->buildUrl($settings['uri']);
    }

    $element['#settings'] = $settings;
    if (!empty($settings['media_switch']) && ($settings['media_switch'] == 'content' || strpos($settings['media_switch'], 'box') !== FALSE)) {
      $this->getMediaSwitch($element, $settings);
    }

    $this->manager->getModuleHandler()->alter('slick_image_pre_render', $element, $settings);
    return $element;
  }

  /**
   * Gets the media switch options.
   *
   * @todo drop for \Drupal\blazy\BlazyManager instead, or do override.
   */
  public function getMediaSwitch(array &$element = [], $settings = []) {
    $type   = isset($settings['type']) ? $settings['type'] : 'image';
    $uri    = $settings['uri'];
    $switch = $settings['media_switch'];

    // Provide relevant URL if it is a lightbox.
    if (strpos($switch, 'box') !== FALSE) {
      $json = ['type' => $type];
      if (!empty($settings['url'])) {
        $url = $settings['url'];
        $json['scheme'] = $settings['scheme'];
        // Force autoplay for media URL on lightboxes, saving another click.
        if ($json['scheme'] == 'soundcloud') {
          if (strpos($url, 'auto_play') === FALSE || strpos($url, 'auto_play=false') !== FALSE) {
            $url = strpos($url, '?') === FALSE ? $url . '?auto_play=true' : $url . '&amp;auto_play=true';
          }
        }
        elseif (strpos($url, 'autoplay') === FALSE || strpos($url, 'autoplay=0') !== FALSE) {
          $url = strpos($url, '?') === FALSE ? $url . '?autoplay=1' : $url . '&amp;autoplay=1';
        }
      }
      else {
        $url = empty($settings['box_style']) ? file_create_url($uri) : $this->manager->entityLoad($settings['box_style'], 'image_style')->buildUrl($uri);
      }

      $classes = ['slick__' . $switch, 'slick__litebox'];
      if ($switch == 'colorbox' && $settings['count'] > 1) {
        $json['rel'] = $settings['id'];
      }
      elseif ($switch == 'photobox' && !empty($settings['url'])) {
        $element['#url_attributes']['rel'] = 'video';
      }

      // Provides lightbox media dimension if so configured.
      if ($type != 'image' && !empty($settings['dimension'])) {
        list($settings['width'], $settings['height']) = array_pad(array_map('trim', explode("x", $settings['dimension'], 2)), 2, NULL);
        $json['width']  = $settings['width'];
        $json['height'] = $settings['height'];
      }

      $element['#url'] = $url;
      $element['#url_attributes']['class'] = $classes;
      $element['#url_attributes']['data-media'] = Json::encode($json);
      $element['#settings']['lightbox'] = $switch;
    }
    elseif ($switch == 'content' && !empty($settings['absolute_path'])) {
      $element['#url'] = $settings['absolute_path'];
    }

    $this->manager->getModuleHandler()->alter('slick_media_switch', $element, $settings);
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
   * Defines image dimensions once as it costs a bit.
   *
   * @todo drop for \Drupal\blazy\BlazyManager instead.
   */
  public function getUrlDimensions(array &$settings = [], $item, $modifier = NULL) {
    if (!is_object($item)) {
      return;
    }

    if (!isset($settings['uri'])) {
      $settings['uri'] = ($entity = $item->entity) && empty($item->uri) ? $entity->getFileUri() : $item->uri;
    }
    if (!empty($modifier)) {
      $style = $this->manager->entityLoad($modifier, 'image_style');
      $settings['image_url'] = $style->buildUrl($settings['uri']);

      if (empty($settings['_dimensions'])) {
        $settings['style_tags'] = $style->getCacheTags();
        $dimensions = [
          'width'  => isset($item->width)  ? $item->width  : '',
          'height' => isset($item->height) ? $item->height : '',
        ];
        $style->transformDimensions($dimensions, $settings['uri']);
        $settings['height']      = $dimensions['height'];
        $settings['width']       = $dimensions['width'];
        $settings['_dimensions'] = TRUE;
      }
    }
    else {
      $settings['image_url'] = $item->entity->url();
      $settings['height']    = $item->height;
      $settings['width']     = $item->width;
    }
  }

  /**
   * Returns the Responsive image cache tags.
   *
   * @todo drop for \Drupal\blazy\BlazyManager instead.
   */
  public function getResponsiveImageCacheTags($responsive_image_style = NULL) {
    $cache_tags = [];
    $image_styles_to_load = [];
    if ($responsive_image_style) {
      $cache_tags = Cache::mergeTags($cache_tags, $responsive_image_style->getCacheTags());
      $image_styles_to_load = $responsive_image_style->getImageStyleIds();
    }

    $image_styles = $this->manager->entityLoadMultiple('image_style', $image_styles_to_load);
    foreach ($image_styles as $image_style) {
      $cache_tags = Cache::mergeTags($cache_tags, $image_style->getCacheTags());
    }
    return $cache_tags;
  }

}
