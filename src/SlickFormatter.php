<?php

/**
 * @file
 * Contains \Drupal\slick\SlickFormatter.
 */

namespace Drupal\slick;

use Drupal\slick\Entity\Slick;
// @todo enable
// use Drupal\blazy\BlazyFormatterManager;

/**
 * Implements SlickFormatterInterface.
 *
 * @todo drop or extend \Drupal\blazy\BlazyFormatterManager.
 */
class SlickFormatter extends SlickImageBase implements SlickFormatterInterface {

  /**
   * {@inheritdoc}
   */
  public function buildSettings(array &$build = [], $items) {
    $settings       = &$build['settings'];
    $field          = $items->getFieldDefinition();
    $entity         = $items->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    $entity_id      = $entity->id();
    $field_name     = $field->getName();
    $field_clean    = str_replace("field_", '', $field_name);
    $target_type    = $field->getFieldStorageDefinition()->getSetting('target_type');
    $optionset_name = $settings['optionset'] ?: 'default';
    $unique         = empty($settings['skin']) ? $optionset_name : $optionset_name . '-' . $settings['skin'];
    $view_mode      = empty($settings['current_view_mode']) ? '_custom' : $settings['current_view_mode'];
    $id             = Slick::getHtmlId("slick-{$entity_type_id}-{$entity_id}-{$field_clean}-{$unique}");
    $internal_path  = $absolute_path = $url = NULL;

    // Deals with UndefinedLinkTemplateException such as paragraphs type.
    // @see #2596385, or fetch the host entity.
    if (!$entity->isNew() && method_exists($entity, 'hasLinkTemplate')) {
      if ($entity->hasLinkTemplate('canonical')) {
        $url = $entity->toUrl();
        $internal_path = $url->getInternalPath();
        $absolute_path = $url->setAbsolute()->toString();
      }
    }

    $settings += [
      'absolute_path'    => $absolute_path,
      'bundle'           => $entity->bundle(),
      'count'            => $items->count(),
      'entity_id'        => $entity_id,
      'entity_type_id'   => $entity_type_id,
      'field_type'       => $field->getType(),
      'field_name'       => $field_name,
      'id'               => $id,
      'internal_path'    => $internal_path,
      'lightbox'         => !empty($settings['media_switch']) && strpos($settings['media_switch'], 'box') !== FALSE,
      'nav'              => !empty($settings['optionset_thumbnail']) && isset($items[1]),
      'target_type'      => $target_type,
      'cache_metadata'   => ['keys' => [$id, $view_mode, $optionset_name]],
    ];

    $build['optionset']   = Slick::load($optionset_name);
    $settings['caption']  = empty($settings['caption']) ? [] : array_filter($settings['caption']);
    $settings['lazy']     = empty($settings['responsive_image_style_id']) ? $build['optionset']->getSetting('lazyLoad') : FALSE;
    $settings['blazy']    = function_exists('blazy_help') && ($settings['lazy'] == 'blazy' || !empty($settings['blazy']));
    $settings['resimage'] = function_exists('responsive_image_get_image_dimensions');

    unset($entity, $field);
  }

}
