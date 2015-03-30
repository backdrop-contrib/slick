<?php
/**
 * @file
 * Hooks provided by the Slick module.
 *
 * Modules may implement any of the available hooks to interact with Slick.
 */

/**
 * Registers Slick skins.
 *
 * This function may live in module file, or my_module.slick.inc if you have
 * many skins.
 *
 * This hook can be used to register skins for the Slick. Skins will be
 * available when configuring the Optionset, Field formatter, or Views style.
 *
 * Slick skins get a unique CSS class to use for styling, e.g.:
 * If you skin name is "my_module_slick_carousel_rounded", the class is:
 * slick--skin--my-module-slick-carousel-rounded
 *
 * A skin can specify some CSS and JS files to include when Slick is displayed.
 *
 * @see hook_hook_info()
 * @see slick_example.module
 */
function hook_slick_skins_info() {
  // The source can be theme or module.
  $theme_path = drupal_get_path('theme', 'my_theme');

  return array(
    'skin_name' => array(
      // Human readable skin name.
      'name' => t('Skin name'),
      // Description of the skin.
      'description' => t('Skin description.'),
      'css' => array(
        // Full path to a CSS file to include with the skin.
        $theme_path . '/css/my-theme.slick.theme--slider.css' => array('weight' => 10),
        $theme_path . '/css/my-theme.slick.theme--carousel.css' => array('weight' => 11),
      ),
      'js' => array(
        // Full path to a JS file to include with the skin.
        $theme_path . '/js/my-theme.slick.theme--slider.js',
        $theme_path . '/js/my-theme.slick.theme--carousel.js',
      ),
    ),
  );
}

/**
 * Alter Slick skins.
 *
 * This function lives in module file, not my_module.slick.inc.
 * Overriding skin CSS can be done via theme.info, hook_css_alter(), or below.
 *
 * @param array $skins
 *   The associative array of skin information from hook_slick_skins_info().
 *
 * @see hook_slick_skins_info()
 * @see slick_example.module
 */
function hook_slick_skins_info_alter(array &$skins) {
  // The source can be theme or module.
  // The CSS is provided by my_theme.
  $path = drupal_get_path('theme', 'my_theme');

  // Modify the default skin's name and description.
  $skins['default']['name'] = t('My Theme: Default');
  $skins['default']['description'] = t('My Theme default skin.');

  // This one won't work.
  // $skins['default']['css'][$path . '/css/slick.theme--base.css'] = array();
  // This one overrides slick.theme--default.css with slick.theme--base.css.
  $skins['default']['css'] = array($path . '/css/slick.theme--base.css' => array('weight' => -22));

  // Overrides skin asNavFor with theme CSS.
  $skins['asnavfor']['name'] = t('My Theme: asnavfor');
  $skins['asnavfor']['css'] = array($path . '/css/slick.theme--asnavfor.css' => array('weight' => 21));

  // Or with the new name.
  $skins['asnavfor']['css'] = array($path . '/css/slick.theme--asnavfor-new.css' => array('weight' => 21));

  // Overrides skin Fullwidth with theme CSS.
  $skins['fullwidth']['name'] = t('My Theme: fullwidth');
  $skins['fullwidth']['css'] = array($path . '/css/slick.theme--fullwidth.css' => array('weight' => 22));
}

/**
 * Alter Slick attach information before they are called.
 *
 * This function lives in module file, not my_module.slick.inc.
 *
 * @param array $attach
 *   The associative array of attach information from slick_attach().
 *
 * @see slick_attach()
 * @see slick_example.module
 */
function hook_slick_attach_info_alter(array &$attach) {
  // Disable inline CSS after copying the output to theme at final stage.
  $attach['attach_inline_css'] = NULL;

  // Disable module JS: slick.load.js to use your own slick JS.
  $attach['attach_module'] = FALSE;

  // Also disable its depencencies, otherwise slick.load.js is still loaded.
  $attach['attach_media'] = FALSE;
  $attach['attach_colorbox'] = FALSE;
}
