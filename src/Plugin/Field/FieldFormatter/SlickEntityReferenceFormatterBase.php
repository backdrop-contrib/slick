<?php

/**
 * @file
 * Contains \Drupal\slick\Plugin\Field\FieldFormatter\SlickEntityReferenceFormatterBase.
 */

namespace Drupal\slick\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Field\Plugin\Field\FieldFormatter\EntityReferenceFormatterBase;
use Drupal\slick\SlickDefault;

/**
 * Base class for slick entity reference formatters.
 *
 * @see \Drupal\slick_media\Plugin\Field\FieldFormatter\SlickMediaFormatter.
 */
abstract class SlickEntityReferenceFormatterBase extends EntityReferenceFormatterBase implements ContainerFactoryPluginInterface {
  use SlickFormatterTrait, SlickConstructorTrait;

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return ['color_field' => ''] + SlickDefault::extendedSettings();
  }

  /**
   * Returns media contents.
   */
  public function buildElements(array &$build = [], $entities, $langcode) {
    $settings = &$build['settings'];
    $item_id  = $settings['item_id'];
    $view_mode = $settings['view_mode'] ?: 'full';

    foreach ($entities as $delta => $entity) {
      // Protect ourselves from recursive rendering.
      static $depth = 0;
      $depth++;
      if ($depth > 20) {
        $this->loggerFactory->get('entity')->error('Recursive rendering detected when rendering entity @entity_type @entity_id. Aborting rendering.', array('@entity_type' => $entity->getEntityTypeId(), '@entity_id' => $entity->id()));
        return $build;
      }

      $settings['delta'] = $delta;
      if ($entity->id()) {
        if ($settings['vanilla']) {
          $build['items'][$delta] = $this->manager()->getEntityTypeManager()->getViewBuilder($entity->getEntityTypeId())->view($entity, $view_mode, $langcode);
        }
        else {
          $this->buildElement($build, $entity, $langcode);
        }

        // Add the entity to cache dependencies so to clear when it is updated.
        $this->manager()->getRenderer()->addCacheableDependency($build['items'][$delta], $entity);
      }
      else {
        $this->referencedEntities = NULL;
        // This is an "auto_create" item.
        $build[$delta] = array('#markup' => $entity->label());
      }

      $depth = 0;
    }

    return $build;
  }

  /**
   * Returns slide contents.
   */
  public function buildElement(array &$build = [], $entity, $langcode) {
    $settings  = &$build['settings'];
    $delta     = $settings['delta'];
    $item_id   = $settings['item_id'];
    $view_mode = $settings['view_mode'] ?: 'full';

    $image = [];
    $this->buildMedia($settings, $entity, $langcode);

    // Main image can be separate image item from video thumbnail for highres.
    $field_image = $settings['image'];
    if ($field_image && isset($entity->$field_image)) {

      /** @var \Drupal\file\Plugin\Field\FieldType\FileFieldItemList $file */
      $file = $entity->get($field_image);

      // Collect cache tags to be added for each item in the field.
      $settings['file_tags'] = $file->referencedEntities()[0]->getCacheTags();
      $settings['uri']       = $file->referencedEntities()[0]->getFileUri();

      /** @var \Drupal\image\Plugin\Field\FieldType\ImageItem $item */
      $element['item']     = $file->get(0);
      $element['settings'] = $settings;

      $image = $this->formatter->getImage($element);
    }

    // Optional image with responsive image, lazyLoad, and lightbox supports.
    $element[$item_id] = $image;
    $element['settings'] = $settings;

    // Captions if so configured.
    $this->getCaption($element, $entity, $langcode);

    // Layouts can be builtin, or field, if so configured.
    if ($layout = $settings['layout']) {
      if (strpos($layout, 'field_') !== FALSE) {
        $settings['layout'] = $this->getFieldString($entity, $layout, $langcode);
      }
      $element['settings']['layout'] = strip_tags($settings['layout']);
    }

    // Classes, if so configured.
    $class = $this->getFieldString($entity, $settings['class'], $langcode);
    $element['settings']['class'] = strip_tags($class);
    $build['items'][$delta] = $element;

    if ($settings['nav']) {
      // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
      $element[$item_id]  = empty($settings['thumbnail_style']) ? [] : $this->formatter->getThumbnail($element['settings']);
      $element['caption'] = $this->getFieldRenderable($entity, $settings['thumbnail_caption'], $view_mode);

      $build['thumb']['items'][$delta] = $element;
    }
  }

  /**
   * Builds slide captions with possible multi-value fields.
   */
  public function getCaption(array &$element = [], $entity, $langcode) {
    $settings  = $element['settings'];
    $view_mode = $settings['view_mode'];

    // Title can be plain text, or link field.
    $field_title = $settings['title'];
    $has_title = $field_title && isset($entity->$field_title);
    if ($has_title && $title = $entity->getTranslation($langcode)->get($field_title)->getValue()) {
      if (!empty($title[0]['value']) && !isset($title[0]['uri'])) {
        // Prevents HTML-filter-enabled text from having bad markups (h2 > p).
        $element['caption']['title']['#markup'] = Xss::filterAdmin($title[0]['value']);
      }
      elseif (isset($title[0]['uri']) && !empty($title[0]['title'])) {
        $element['caption']['title'] = $this->getFieldRenderable($entity, $field_title, $view_mode)[0];
      }
    }

    // Other caption fields, if so configured.
    if (!empty($settings['caption'])) {
      $caption_items = [];
      foreach ($settings['caption'] as $i => $field_caption) {
        if (!isset($entity->$field_caption)) {
          continue;
        }
        $caption_items[$i] = $this->getFieldRenderable($entity, $field_caption, $view_mode);
      }
      if ($caption_items) {
        $element['caption']['data'] = $caption_items;
      }
    }

    // Link, if so configured.
    $field_link = $settings['link'];
    if ($field_link && isset($entity->$field_link)) {
      $links = $this->getFieldRenderable($entity, $field_link, $view_mode);
      // Only simplify markups for known formatters registered by link.module.
      if ($links && in_array($links['#formatter'], ['link'])) {
        $links = [];
        foreach ($entity->$field_link as $i => $link) {
          $links[$i] = $link->view($view_mode);
        }
      }
      $element['caption']['link'] = $links;
    }

    $element['caption']['overlay'] = empty($settings['overlay']) ? [] : $this->getOverlay($element, $entity, $langcode);
  }

  /**
   * Builds slide overlay placed within the caption.
   */
  public function getOverlay(array &$element = [], $entity, $langcode) {
    return [];
  }

  /**
   * Collects media definitions.
   */
  public function buildMedia(array &$settings = [], $entity, $langcode) {
    $settings['bundle']         = $entity->bundle();
    $settings['entity_url']     = $entity->url();
    $settings['id']             = $entity->id();
    $settings['target_bundles'] = $this->getFieldSetting('handler_settings')['target_bundles'];

    // @todo get 'type' independent from bundle names: image, video, audio.
    $settings['type']           = $entity->bundle();
  }

  /**
   * Returns the string value of the fields: link or text.
   */
  public function getFieldString($entity, $field_name = '', $langcode, $formatted = FALSE) {
    $value = '';
    if ($field_name && isset($entity->$field_name)) {
      $values = $entity->getTranslation($langcode)->get($field_name)->getValue();
      if (!empty($values[0]['value'])) {
        $value = $values[0]['value'];
      }
      elseif (isset($values[0]['uri']) && !empty($values[0]['title'])) {
        $value = $values[0]['uri'];
      }
    }
    return $value;
  }

  /**
   * Returns the formatted renderable array of the field.
   */
  public function getFieldRenderable($entity, $field_name = '', $view_mode) {
    $has_field = $field_name && isset($entity->$field_name) && !empty($entity->$field_name->view($view_mode)[0]);
    return $has_field ? $entity->$field_name->view($view_mode) : [];
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $element  = [];
    $admin    = $this->admin();
    $bundles  = $this->getFieldSetting('handler_settings')['target_bundles'];
    $strings  = $admin->getFieldOptions($bundles, ['text', 'string', 'list_string']);
    $texts    = $admin->getFieldOptions($bundles, ['text', 'text_long', 'string', 'string_long', 'link']);

    $definition = [
      'captions'          => $admin->getFieldOptions($bundles),
      'classes'           => $strings,
      'current_view_mode' => $this->viewMode,
      'fieldable_form'    => TRUE,
      'images'            => $admin->getFieldOptions($bundles, ['image']),
      'layouts'           => $strings,
      'links'             => $admin->getFieldOptions($bundles, ['text', 'string', 'link']),
      'multimedia'        => TRUE,
      'settings'          => $this->getSettings(),
      'switchers'         => TRUE,
      'target_bundles'    => $bundles,
      'target_type'       => $this->getFieldSetting('target_type'),
      'thumb_captions'    => $texts,
      'titles'            => $texts,
      'vanilla'           => TRUE,
    ];

    $admin->openingForm($element, $definition);
    $admin->imageForm($element, $definition);
    $admin->closingForm($element, $definition);

    $layout_description = $element['layout']['#description'];
    $element['layout']['#description'] = t('Create a dedicated List (text - max number 1) field related to the caption placement to have unique layout per slide with the following supported keys: top, right, bottom, left, center, center-top, etc. Be sure its formatter is Key.') . ' ' . $layout_description;

    $element['media_switch']['#options']['media'] = t('Image to iframe');
    $element['media_switch']['#description'] .= ' ' . t('Be sure the enabled fields here are not hidden/disabled at its view mode.');
    $element['image']['#description'] .= ' ' . t('For video/audio, this allows separate highres image.');
    $element['caption']['#description'] = t('Check fields to be treated as captions.');

    return $element;
  }

}
