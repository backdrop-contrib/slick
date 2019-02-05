<?php

namespace Drupal\slick\Plugin\Field\FieldFormatter;

use Drupal\blazy\Plugin\Field\FieldFormatter\BlazyFormatterTrait;
use Drupal\slick\Form\SlickAdmin;

/**
 * A Trait common for slick formatters.
 */
trait SlickFormatterTrait {

  use BlazyFormatterTrait;

  /**
   * The slick field formatter manager.
   *
   * @var \Drupal\slick\SlickAdminInterface
   */
  protected $admin;

  /**
   * Returns the slick admin service shortcut.
   */
  public function admin() {
    if (!isset($this->admin)) {
      $this->admin = new SlickAdmin($this->manager);
    }
    return $this->admin;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements($items, $entity) {
    $entities = $this->getEntitiesToView($items);

    // Early opt-out if the field is empty.
    if (empty($entities)) {
      return [];
    }

    // Collects specific settings to this formatter.
    $this->entity = $entity;
    $settings = $this->buildSettings();
    $build = ['settings' => $settings];
    $this->formatter()->buildSettings($build, $items, $entity);

    // Build the elements.
    $this->buildElements($build, $entities);

    // Supports Blazy multi-breakpoint images if provided.
    if (isset($build['items'][0]) && empty($settings['vanilla'])) {
      $this->formatter()->isBlazy($build['settings'], $build['items'][0]);
    }
    // If using 0, or directly passed like D8, taken over by theme_field().
    $element = $this->manager()->build($build);
    return $element;
  }

}
