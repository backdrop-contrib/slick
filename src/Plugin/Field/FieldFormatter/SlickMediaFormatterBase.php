<?php

namespace Drupal\slick\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Logger\LoggerChannelFactoryInterface;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\blazy\BlazyOEmbed;
use Drupal\blazy\Plugin\Field\FieldFormatter\BlazyMediaFormatterBase;
use Drupal\slick\SlickFormatterInterface;
use Drupal\slick\SlickManagerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Base class for Slick media formatters with field details, and oEmbed support.
 *
 * This is not functional yet till a new formatter is created, or `Slick Media`
 * extends this. The candidate name if included in the main module is
 * `Slick oEmbed`, or just `Slick Media (with oEmbed)`` with machine name
 * `slick_oembed` to avoid conflict with existing `slick_media`. Alternatively
 * replace `Slick Media` with a similar formatter of the main module, and
 * provide a hook_update().
 *
 * @see Drupal\slick_media\Plugin\Field\FieldFormatter\SlickMediaFormatter
 * For the proof of concept, Slick Media 8.x-2.x can extend this to have oEmbed.
 */
abstract class SlickMediaFormatterBase extends BlazyMediaFormatterBase implements ContainerFactoryPluginInterface {

  use SlickFormatterTrait;

  /**
   * The image style entity storage.
   *
   * @var \Drupal\Core\Entity\EntityStorageInterface
   */
  protected $imageStyleStorage;

  /**
   * Constructs a BlazyFormatter object.
   */
  public function __construct($plugin_id, $plugin_definition, FieldDefinitionInterface $field_definition, array $settings, $label, $view_mode, array $third_party_settings, LoggerChannelFactoryInterface $logger_factory, BlazyOEmbed $blazy_oembed, SlickFormatterInterface $formatter, EntityStorageInterface $image_style_storage, SlickManagerInterface $manager) {
    parent::__construct($plugin_id, $plugin_definition, $field_definition, $settings, $label, $view_mode, $third_party_settings, $logger_factory, $blazy_oembed, $formatter);

    $this->imageStyleStorage = $image_style_storage;
    $this->formatter         = $formatter;
    $this->manager           = $manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $plugin_id,
      $plugin_definition,
      $configuration['field_definition'],
      $configuration['settings'],
      $configuration['label'],
      $configuration['view_mode'],
      $configuration['third_party_settings'],
      $container->get('logger.factory'),
      $container->get('blazy.oembed'),
      $container->get('slick.formatter'),
      $container->get('entity.manager')->getStorage('image_style'),
      $container->get('slick.manager')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getScopedFormElements() {
    return [
      'namespace' => 'slick',
    ] + parent::getScopedFormElements();
  }

}
