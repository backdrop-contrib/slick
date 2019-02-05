<?php

/**
 * @file
 * Slick default optionset API include file.
 */

use Drupal\slick\Entity\Slick;

/**
 * Implements hook_slick_optionsets().
 */
function slick_slick_optionsets() {
  $optionsets = [];

  $values = [
    'name' => 'default',
    'label' => 'Default',
    'breakpoints' => 0,
    'skin' => '',
    'group' => '',
    'optimized' => 0,
  ];
  $optionset = Slick::create($values);
  $optionset->disabled = TRUE;
  $optionset->api_version = 3;
  $optionsets['default'] = $optionset;

  return $optionsets;
}
