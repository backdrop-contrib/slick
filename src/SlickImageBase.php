<?php

/**
 * @file
 * Contains \Drupal\slick\SlickImage.
 */

namespace Drupal\slick;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Cache\Cache;
use Drupal\Component\Serialization\Json;
use Drupal\image\Entity\ImageStyle;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\slick\SlickManagerInterface;

/**
 * Provides base lazyloaded image and thumbnail building.
 *
 * @see \Drupal\slick\SlickFormatter
 * @todo Port D7 SlickViews lazyload capability using this class.
 */
abstract class SlickImageBase {

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
   */
  public function getImage($build = []) {
    $tags     = [];
    $item     = $build['item'];
    $settings = &$build['settings'];
    $media    = &$build['media'];

    $image_style = $settings['image_style'];
    $dimensions  = $this->setDimensions($item, $image_style, $media['uri']);
    $media       = array_merge($media, $dimensions);
    $settings    = array_merge($settings, $media);
    $delta       = $media['delta'];

    // Collect cache tags to be added for each item in the field.
    if (!empty($image_style)) {
      $style = ImageStyle::load($image_style);
      $tags  = $style->getCacheTags();
    }

    $image = [
      '#theme'      => 'slick_image',
      '#item'       => [],
      '#delta'      => $delta,
      '#build'      => $build,
      '#pre_render' => [[$this, 'preRenderImage']],
      '#cache'      => ['tags' => $tags],
    ];

    // Build the slide with picture, lightbox or multimedia supports.
    return [
      '#theme'    => 'slick_media',
      '#item'     => $image,
      '#delta'    => $delta,
      '#settings' => $settings,
    ];
  }

  /**
   * Builds the Slick image as a structured array ready for ::renderer().
   */
  public function preRenderImage($element) {
    $build = $element['#build'];
    $item  = $build['item'];
    unset($element['#build']);

    if (empty($item)) {
      return [];
    }

    $media    = $build['media'];
    $settings = $build['settings'];
    $resimage = function_exists('responsive_image_get_image_dimensions');

    // Extract field item attributes for the theme function, and unset them
    // from the $item so that the field template does not re-render them.
    $item_attributes = $item->_attributes;
    unset($item->_attributes);

    $element['#item'] = $item;
    $element['#attributes'] = $item_attributes;

    // Responsive image integration.
    if ($resimage && !empty($settings['responsive_image_style'])) {
      $responsive_image_style = $this->manager->load($settings['responsive_image_style'], 'responsive_image_style');
      $settings['responsive_image_style_id'] = $responsive_image_style->id() ?: '';
      $element['#cache'] = [
        'tags' => $this->getResponsiveImageCacheTags($responsive_image_style),
      ];
    }
    elseif (!empty($media['width'])) {
      // Allows multiple dimensions with just a single Media entity view mode.
      $element['#attributes']['height'] = $media['height'];
      $element['#attributes']['width']  = $media['width'];
    }

    if (!empty($settings['thumbnail_style'])) {
      $element['#attributes']['data-thumb'] = ImageStyle::load($settings['thumbnail_style'])->buildUrl($media['uri']);
    }

    $element['#settings'] = $settings;
    $switch = $settings['media_switch'];
    if (!empty($switch) && ($switch == 'content' || strpos($switch, 'box') !== FALSE)) {
      $element = NestedArray::mergeDeep($element, $this->getMediaSwitch($media, $settings));
    }
    return $element;
  }

  /**
   * Gets the media switch options.
   */
  public function getMediaSwitch($media = [], $settings = []) {
    $image  = [];
    $type   = isset($media['type']) ? $media['type'] : 'image';
    $uri    = $media['uri'];
    $switch = $settings['media_switch'];

    // Provide relevant URL if it is a lightbox.
    if (strpos($switch, 'box') !== FALSE) {
      $json = ['type' => $type];
      if (!empty($media['url'])) {
        $url = $media['url'];
        $json['scheme'] = $media['scheme'];
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
        $url = empty($settings['box_style']) ? file_create_url($uri) : ImageStyle::load($settings['box_style'])->buildUrl($uri);
      }

      $classes  = ['slick__' . $switch, 'slick__litebox'];
      if ($switch == 'colorbox' && $settings['count'] > 1) {
        $json['rel'] = $settings['id'];
      }
      elseif ($switch == 'photobox' && !empty($media['url'])) {
        $image['#settings']['url']['attributes']['rel'] = 'video';
      }
      elseif ($switch == 'slickbox') {
        $classes = ['slick__box', 'slick__litebox'];
        $json['entity_id'] = $settings['entity_id'];
        if (!empty($settings['absolute_path']) && !empty($settings['use_ajax'])) {
          $url = $settings['absolute_path'];
          $json['ajax'] = TRUE;
        }
      }

      if ($type != 'image' && !empty($settings['dimension'])) {
        list($media['width'], $media['height']) = array_pad(array_map('trim', explode("x", $settings['dimension'], 2)), 2, NULL);
        $json['width']  = $media['width'];
        $json['height'] = $media['height'];
      }

      $image['#url'] = $url;
      $image['#settings']['url']['attributes']['class'] = $classes;
      $image['#settings']['url']['attributes']['data-media'] = Json::encode($json);
      $image['#settings']['lightbox'] = $switch;
    }
    elseif ($switch == 'content' && !empty($settings['absolute_path'])) {
      $image['#url'] = $settings['absolute_path'];
    }

    $this->manager->getModuleHandler()->alter('slick_media_switch_info', $image, $settings);
    return $image;
  }

  /**
   * Gets the thumbnail image.
   */
  public function getThumbnail($slide = []) {
    if (empty($slide['media']['uri'])) {
      return [];
    }
    $thumbnail = [
      '#theme'      => 'image_style',
      '#style_name' => $slide['settings']['thumbnail_style'],
      '#uri'        => $slide['media']['uri'],
    ];

    foreach (['height', 'width', 'alt', 'title'] as $data) {
      $thumbnail["#$data"] = isset($slide['media'][$data]) ? $slide['media'][$data] : NULL;
    }
    return $thumbnail;
  }

  /**
   * Transform image dimensions based on the given image style.
   */
  public function setDimensions($item, $image_style = '', $uri = '') {
    $media = [];
    if ($image_style && $uri) {
      $style = ImageStyle::load($image_style);
      $dimensions = array(
        'width' => isset($item->width) ? $item->width : '',
        'height' => isset($item->height) ? $item->height : '',
      );
      $style->transformDimensions($dimensions, $uri);
      $media['height'] = $dimensions['height'];
      $media['width'] = $dimensions['width'];
      $media['dimensions'] = TRUE;
    }
    return $media;
  }

  /**
   * Returns the Responsive image cache tags.
   */
  public function getResponsiveImageCacheTags($responsive_image_style = NULL) {
    $image_styles_to_load = [];
    $cache_tags = [];
    if ($responsive_image_style) {
      $cache_tags = Cache::mergeTags($cache_tags, $responsive_image_style->getCacheTags());
      $image_styles_to_load = $responsive_image_style->getImageStyleIds();
    }

    $image_styles = $this->manager->loadMultiple('image_style', $image_styles_to_load);
    foreach ($image_styles as $image_style) {
      $cache_tags = Cache::mergeTags($cache_tags, $image_style->getCacheTags());
    }
    return $cache_tags;
  }

}
