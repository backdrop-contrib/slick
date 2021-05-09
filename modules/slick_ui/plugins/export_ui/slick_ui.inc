<?php

/**
 * @file
 * Export interface plugin.
 */

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'slick_optionset',

  // Define user interface texts.
  'title singular' => t('optionset'),
  'title plural' => t('optionsets'),
  'title singular proper' => t('Slick carousel optionset'),
  'title plural proper' => t('Slick carousel optionsets'),

  // Define our handler.
  'handler' => array(
    'class' => 'SlickUi',
    'parent' => 'ctools_export_ui',
  ),
);
