<?php

/**
 * @file
 * Contains \Drupal\slick\Plugin\Field\FieldFormatter\SlickEntityReferenceFormatterBase.
 */

namespace Drupal\slick\Plugin\Field\FieldFormatter;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\blazy\Dejavu\BlazyEntityReferenceBase;

/**
 * Base class for slick entity reference formatters.
 *
 * @see \Drupal\slick_media\Plugin\Field\FieldFormatter\SlickMediaFormatter.
 */
abstract class SlickEntityReferenceFormatterBase extends BlazyEntityReferenceBase implements ContainerFactoryPluginInterface {
  use SlickFormatterTrait, SlickConstructorTrait;

}
