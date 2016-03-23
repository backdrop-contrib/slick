<?php

/**
 * @file
 * Contains \Drupal\slick\Plugin\Field\FieldFormatter\SlickImageFormatter.
 */

namespace Drupal\slick\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\image\Plugin\Field\FieldFormatter\ImageFormatterBase;
use Drupal\slick\SlickDefault;

/**
 * Plugin implementation of the 'slick image' formatter.
 *
 * @FieldFormatter(
 *   id = "slick_image",
 *   label = @Translation("Slick carousel"),
 *   description = @Translation("Display the images as a Slick carousel."),
 *   field_types = {"image"},
 *   quickedit = {"editor" = "disabled"}
 * )
 */
class SlickImageFormatter extends ImageFormatterBase implements ContainerFactoryPluginInterface {
  use SlickFormatterTrait, SlickConstructorTrait;

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return SlickDefault::imageSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $files = $this->getEntitiesToView($items, $langcode);

    // Early opt-out if the field is empty.
    if (empty($files)) {
      return [];
    }

    // Collects specific settings to this formatter.
    $settings = $this->getSettings();

    // Prepare integration with Blazy.
    $settings['item_id']                  = 'slide';
    $settings['theme_hook_image']         = 'slick_image';
    $settings['theme_hook_image_wrapper'] = 'slick_media';

    // Build the settings.
    $build = ['settings' => $settings];
    $this->formatter->buildSettings($build, $items);

    // Build the elements.
    $this->buildElements($build, $files);

    return $this->manager()->build($build);
  }

  /**
   * Build the slick carousel elements.
   */
  public function buildElements(array &$build = [], $files) {
    $settings = &$build['settings'];
    $item_id  = $settings['item_id'];

    foreach ($files as $delta => $file) {
      /* @var Drupal\image\Plugin\Field\FieldType\ImageItem $item */
      $item = $file->_referringItem;

      $settings['uri']       = ($entity = $item->entity) && empty($item->uri) ? $entity->getFileUri() : $item->uri;
      $settings['delta']     = $delta;
      $settings['file_tags'] = $file->getCacheTags();
      $settings['type']      = 'image';

      $element = ['item' => $item, 'settings' => $settings];

      if (!empty($settings['caption'])) {
        foreach ($settings['caption'] as $caption) {
          $element['caption'][$caption] = empty($item->$caption) ? [] : ['#markup' => Xss::filterAdmin($item->$caption)];
        }
      }

      // Image with responsive image, lazyLoad, and lightbox supports.
      $element[$item_id] = $this->formatter->getImage($element);
      $build['items'][$delta] = $element;

      if ($settings['nav']) {
        // Thumbnail usages: asNavFor pagers, dot, arrows, photobox thumbnails.
        $element[$item_id] = empty($settings['thumbnail_style']) ? [] : $this->formatter->getThumbnail($element['settings']);

        $caption = $settings['thumbnail_caption'];
        $element['caption'] = empty($item->$caption) ? [] : ['#markup' => Xss::filterAdmin($item->$caption)];

        $build['thumb']['items'][$delta] = $element;
      }
      unset($element);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $element    = [];
    $captions   = ['title' => t('Title'), 'alt' => t('Alt')];
    $definition = [
      'current_view_mode' => $this->viewMode,
      'captions'          => $captions,
      'settings'          => $this->getSettings(),
      'thumb_captions'    => $captions,
      'switchers'         => TRUE,
    ];

    $this->admin()->openingForm($element, $definition);
    $this->admin()->imageForm($element, $definition);
    $this->admin()->closingForm($element, $definition);
    return $element;
  }

}
