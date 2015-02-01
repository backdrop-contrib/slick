<?php

/**
 * @file
 * Slick development admin.
 *
 * Contains optional functions called only if needed by admin pages.
 */

/**
 * Form builder; Form for advanced module settings.
 */
function slick_devel_form_settings() {
  $form = array();

  $form['library'] = array(
    '#type' => 'fieldset',
    '#title' => 'Library',
    '#description' => t("Unless you are helping to develop the Slick module, all these are not needed to run Slick. Requires slick > 1.4."),
  );

  $form['library']['slick_devel'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable development mode'),
    '#description' => t('Load the development version of the Slick library. Only useful to test new features of the library. Leave it unchecked at production.'),
    '#default_value' => variable_get('slick_devel', FALSE),
  );

  $form['library']['slick_devel_disable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable module slick.load.js'),
    '#default_value' => variable_get('slick_devel_disable', FALSE),
  );

  $form['library']['slick_devel_replace'] = array(
    '#type' => 'checkbox',
    '#title' => t('Replace the slick.load.js with development version: slick.load.devel.js'),
    '#default_value' => variable_get('slick_devel_replace', FALSE),
    '#states' => array(
      'invisible' => array(
        array(':input[name="slick_devel_disable"]' => array('checked' => TRUE)),
      ),
    ),
  );

  return system_settings_form($form);
}
