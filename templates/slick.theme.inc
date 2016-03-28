<?php

/**
 * @file
 * Hooks and preprocess functions for the Slick module.
 */

use Drupal\Core\Template\Attribute;
use Drupal\Component\Utility\Unicode;
use Drupal\Component\Utility\Html;
use Drupal\Component\Serialization\Json;
use Drupal\blazy\Blazy;
use Drupal\slick\Entity\Slick;

/**
 * Prepares variables for slick.html.twig templates.
 */
function template_preprocess_slick(&$variables) {
  $element   = $variables['element'];
  $defaults  = Slick::htmlSettings();
  $settings  = isset($element['#settings']) ? array_merge($defaults, $element['#settings']) : $defaults;
  $customs   = isset($element['#options']) ? $element['#options'] : [];
  $optionset = isset($element['#optionset']) ? $element['#optionset'] : Slick::load($settings['optionset']);
  $js        = $customs ? array_merge($optionset->getSettings(), $customs) : $optionset->getSettings();
  $display   = $variables['display'] = $settings['display'];

  // Prepare attributes.
  $attributes = &$variables['attributes'];
  $content_attributes = new Attribute();

  if (!isset($attributes['id'])) {
    $id = Slick::getHtmlId('slick', $settings['id']);
    $attributes['id'] = $display == 'thumbnail' ? $id . '-thumbnail' : $id;
  }

  // Sniffs for Views to allow block__no_wrapper, views__no_wrapper, etc.
  if ($settings['view_name'] && $settings['current_view_mode']) {
    $settings['attributes']['class'][] = Html::cleanCssIdentifier('slick--view--' . $settings['view_name']);
    $settings['attributes']['class'][] = Html::cleanCssIdentifier('slick--view--' . $settings['view_name'] . '--' . $settings['current_view_mode']);
  }

  // @see SlickManager::buildGrid(), and this should make sense.
  $settings['count']   = isset($settings['count']) ? $settings['count'] : count($element['#items']);
  $settings['unslick'] = $settings['unslick'] || $settings['count'] == 1;

  foreach (['asNavFor', 'nextArrow', 'prevArrow', 'downArrowTarget', 'downArrowOffset', 'slidesToShow'] as $key) {
    $settings[$key] = isset($js[$key]) ? $js[$key] : FALSE;
  }

  $settings['lazy'] = isset($settings['lazy']) ? $settings['lazy'] : $js['lazyLoad'];

  // Blazy can still lazyload an unslick.
  if ($display != 'thumbnail' && $settings['lazy'] == 'blazy') {
    $js['lazyLoad'] = 'blazy';
    $blazy = empty($settings['blazy_data']) ? [] : $settings['blazy_data'];
    $blazy['ratio'] = $settings['ratio'];
    if (!empty($settings['width']) && !$blazy['ratio']) {
      $blazy['max'] = [$settings['width'], $settings['height']];
    }
    $attributes['data-blazy'] = Json::encode($blazy);
  }

  // Prevents broken slick when only one item given, or an enforced unslick.
  if (!$settings['unslick']) {
    $content_attributes->setAttribute('id', $attributes['id'] . '-slider');

    $settings['arrow']['attributes'] = new Attribute();
    if ($display == 'main' && $js['downArrow'] && $js['downArrowTarget']) {
      $settings['downArrow']['attributes'] = new Attribute();
    }

    // focusOnSelect won't work with empty slide value, so add proper selector.
    if (empty($js['slide']) && $js['focusOnSelect']) {
      $js['slide'] = ($js['rows'] == 1 && $js['slidesPerRow'] == 1) ? '.slick__slide' : $js['slide'];
    }

    if ($js['rtl']) {
      $content_attributes->setAttribute('dir', 'rtl');
    }

    // Add the configuration as JSON object into the slick container.
    if ($json = $optionset->removeDefaultValues($js)) {
      $content_attributes->setAttribute('data-slick', Json::encode($json));
    }
  }

  // Pass settings and attributes to twig.
  $variables['settings'] = $settings;
  $variables['content_attributes'] = $content_attributes;

  // Process individual item.
  $variables['items'] = [];
  foreach ($element['#items'] as $delta => $item) {
    $settings['current_item'] = $display;

    $theme = $settings['vanilla'] ? 'vanilla' : ($display == 'thumbnail' ? 'thumbnail' : 'slide');
    $slide = _slick_build_slide($item, $delta, $settings, 'slick_' . $theme);
    $variables['items'][$delta] = $slide;
    unset($slide);
  }
}

/**
 * Re-uses individual slide building for grid, slide, thumbnail, vanilla.
 */
function _slick_build_slide($item, $delta = 0, $settings = array(), $theme = 'slick_slide') {
  $settings = isset($item['settings']) ? array_merge($settings, $item['settings']) : $settings;
  unset($item['settings']);
  return [
    '#theme'    => $theme,
    '#item'     => $item,
    '#delta'    => $delta,
    '#settings' => $settings,
  ];
}

/**
 * Prepares variables for slick-wrapper.html.twig templates.
 */
function template_preprocess_slick_wrapper(&$variables) {
  foreach (['attributes', 'items', 'settings'] as $key) {
    $variables[$key] = isset($variables['element']["#$key"]) ? $variables['element']["#$key"] : [];
  }
}

/**
 * Prepares common variables for slick item templates.
 */
function _slick_preprocess_slick_item(&$variables) {
  foreach (['attributes', 'delta', 'item', 'settings'] as $key) {
    $variables[$key] = isset($variables['element']["#$key"]) ? $variables['element']["#$key"] : [];
  }
}

/**
 * Prepares variables for slick-vanilla.html.twig templates.
 */
function template_preprocess_slick_vanilla(&$variables) {
  _slick_preprocess_slick_item($variables);
}

/**
 * Prepares variables for slick-thumbnail.html.twig templates.
 */
function template_preprocess_slick_thumbnail(&$variables) {
  _slick_preprocess_slick_item($variables);
}

/**
 * Prepares variables for slick-slide.html.twig templates.
 */
function template_preprocess_slick_slide(&$variables) {
  _slick_preprocess_slick_item($variables);

  // All slide types -- main, thumbnail, grid, overlay -- may have captions.
  foreach (['alt', 'data', 'link', 'overlay', 'title'] as $key) {
    $variables['item']['caption'][$key] = isset($variables['item']['caption'][$key]) ? $variables['item']['caption'][$key] : [];
  }

  $item     = &$variables['item'];
  $settings = &$variables['settings'];

  // split: Split image from captions if we do have captions, and main image.
  // fullwidth: If full skins, add wrappers to hold caption and overlay.
  // detroy: Remove .slide__content if it is an enforced unslick grid.
  // wrapper: Don't add divities for a single item to have clean markups.
  $item['slide']         = isset($item['slide']) ? $item['slide'] : [];
  $item['caption']       = array_filter($item['caption']);
  $settings['split']     = !empty($item) && $settings['caption'];
  $settings['data']      = !empty($item['caption']['alt']) || !empty($item['caption']['title']) || !empty($item['caption']['data']);
  $settings['fullwidth'] = !empty($settings['skin']) && strpos($settings['skin'], 'full') !== FALSE;
  $settings['detroy']    = $settings['current_item'] == 'main' && !empty($settings['grid']) && !empty($settings['unslick']);
  $settings['wrapper']   = $settings['count'] > 1 && $settings['current_item'] != 'grid';
}

/**
 * Prepares variables for slick-grid.html.twig templates.
 */
function template_preprocess_slick_grid(&$variables) {
  $variables['settings'] = $variables['element']['#settings'];

  $variables['items'] = [];
  foreach ($variables['element']['#items'] as $delta => $item) {
    $variables['settings']['current_item'] = 'grid';

    $slide['slide'] = _slick_build_slide($item, $delta, $variables['settings']);
    $slide['attributes'] = new Attribute();
    $variables['items'][$delta] = $slide;
    unset($slide);
  }
}

/**
 * Prepares variables for slick-media.html.twig templates.
 *
 * @todo merge with slick_image?
 */
function template_preprocess_slick_media(&$variables) {
  _slick_preprocess_slick_item($variables);

  // Configure attributes for containing elements.
  $content_attributes = [];
  $settings           = &$variables['settings'];
  $settings['player'] = $settings['ratio_clean'] = FALSE;

  // Pure CSS responsive iframes/images based on aspect ratio.
  if (!empty($settings['ratio'])) {
    $settings['ratio_clean'] = str_replace(':', '', 'media--ratio--' . $settings['ratio']);

    if (!empty($settings['height']) && in_array($settings['ratio'], ['enforced', 'fluid'])) {
      $variables['attributes']['style'] = 'padding-bottom: ' . round((($settings['height'] / $settings['width']) * 100), 2) . '%';
    }
  }

  // The media audio/video files can be iframed/lightboxed/switched.
  $variables['content'] = $variables['element']['#children'];
  if (in_array($settings['type'], ['video', 'audio']) && !empty($settings['url'])) {
    // item  : If iframe switch disabled, fallback to just iframe, remove image.
    // player: If no colorbox/photobox, it is an image to iframe switcher.
    // data- : Gets consistent with colorbox to share JS manipulation.
    $variables['content']             = empty($settings['media_switch']) ? [] : $variables['content'];
    $settings['player']               = !$settings['lightbox'];
    $content_attributes['data-media'] = Json::encode(['type' => $settings['type'], 'scheme' => $settings['scheme']]);
    $content_attributes['data-lazy']  = $settings['url'];
    $content_attributes['src']        = empty($settings['iframe_lazy']) ? $settings['url'] : 'about:blank';
  }

  $variables['content_attributes'] = new Attribute($content_attributes);
}

/**
 * Prepares variables for slick-image.html.twig template.
 *
 * @todo consider extending Blazy::buildAttributes() if easier to merge.
 */
function template_preprocess_slick_image(array &$variables) {
  _slick_preprocess_slick_item($variables);

  $element  = $variables['element'];
  $item     = $variables['item'];
  $settings = &$variables['settings'];
  $uri      = ($entity = $item->entity) && empty($item->uri) ? $entity->getFileUri() : $item->uri;

  $variables['noscript']   = '';
  $variables['attributes'] = isset($element['#item_attributes']) ? $element['#item_attributes'] : [];
  $variables['url']        = isset($element['#url']) ? $element['#url'] : '';

  $attributes = &$variables['attributes'];
  $attributes['class'][] = 'media__image';
  $url_attributes = isset($element['#url_attributes']) ? $element['#url_attributes'] : [];

  // Do not output an empty 'title' attribute.
  if (Unicode::strlen($item->title) != 0) {
    $variables['image']['#title'] = $item->title;
  }

  // Check whether we have responsive image, or plain one.
  $variables['image']['#uri'] = $uri;
  $variables['image']['#alt'] = isset($item->alt) ? $item->alt : NULL;
  if (!empty($settings['responsive_image_style_id'])) {
    $variables['image']['#type'] = 'responsive_image';
    $variables['image']['#responsive_image_style_id'] = $settings['responsive_image_style_id'];
  }
  else {
    $variables['image']['#theme'] = 'image';
    $settings['image_url'] = isset($settings['image_url']) ? $settings['image_url'] : $item->entity->url();

    // Provide lazyloaded image with noscript for the first slide only.
    // Slick is unslick when count = 1, otherwise broken.
    // This is a bit problematic, unless using Blazy.
    if (($settings['count'] > 1 && $settings['lazy']) || !empty($settings['blazy'])) {
      $variables['noscript']      = $settings['delta'] == 0 ? $variables['image'] : '';
      $attributes['class'][]      = 'lazy';
      $variables['image']['#uri'] = 'data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7';

      // Build multi-serving images based on breakpoints if provided.
      Blazy::buildBreakpointAttributes($attributes, $settings);
    }

    // @todo drop if extending Blazy::buildAttributes().
    if (!empty($settings['ratio'])) {
      $attributes['class'][] = 'media__element';
      if (!empty($variables['url'])) {
        $url_attributes['class'][] = 'media__element';
      }
    }
  }

  $variables['image']['#attributes'] = $attributes;

  $settings['player'] = isset($settings['player']) ? $settings['player'] : FALSE;
  $settings['icon']   = isset($settings['icon']) ? $settings['icon'] : [];
  if (empty($settings['icon']) && !empty($settings['lightbox'])) {
    $settings['icon']['#markup'] = '<span class="media__icon media__icon--litebox"></span>';
  }

  // URL can be entity, or lightbox URL different from the content image URL.
  $variables['url_attributes'] = new Attribute($url_attributes);
}
