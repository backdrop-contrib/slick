<?php
/**
 * @file
 * Hooks and API provided by the Slick module.
 *
 * Modules may implement any of the available hooks to interact with Slick.
 */

/**
 * Slick may be configured using the web interface via sub-modules.
 *
 * However if you want to code it, use slick_build(), or build it from the
 * available data for more advanced slick such as asNavFor, see slick_fields.
 *
 * The example is showing a customized views-view-unformatted--ticker.tpl.php.
 * Practically any content-related .tpl.php file where you have data to print.
 * Do preprocess, or here a direct .tpl.php manipulation for quick illustration.
 *
 * The goal is to create a vertical newsticker, or tweets, with pure text only.
 * First, create an unformatted Views block, says 'Ticker' containing ~ 10
 * titles, or any data for the contents -- using EFQ, or static array will do.
 */
// 1.
// Provides HTML settings with optionset name and ID, none of JS related.
// See slick_get_element_default_settings() for more supported keys.
// To add JS key:value pairs, use #options at theme_slick() below instead.
$id = 'slick-ticker';
$settings = array(
// Optional optionset name, otherwise fallback to default.
// 'optionset' => 'default',
// Optional skin name fetched from hook_slick_skins_info(), otherwise none.
// 'skin' => 'default',
// Note we add attributes to the settings, not as theme key here, to allow
// various scenarios before being passed to the actual #attributes property.
  'attributes' => array(
    'id' => $id,
  ),
);

// 3.
// Prepare $items contents, note 'slide' key is to hold the actual slide
// which can be pure and simple text, or any image/media file. Meaning $rows can
// be text only, or image/audio/video, or a combination of both.
// To add caption/overlay, use 'caption' key with the supported sub-keys:
// title, alt, link, layout, overlay, editor, or data for complex content.
// You must sanitize each sub-key yourself accordingly.
// See template_preprocess_slick_item() for more info.
$items = array();
foreach ($rows as $row) {
  $items[] = array(
    'slide' => $row,
    // If the slide is image, to add text caption, use:
    // 'caption' => 'some-caption data',
  );
}

// 4.
// Optional JS and CSS assets loader, see slick_attach(). An empty array should
// suffice for the most basic slick with no skin at all.
$attach = array();

// 5.
// Optional specific Slick JS options, if no optionset provided above.
// Play with speed and options to achieve desired result.
// @see slick_get_options()
$options = array(
  'arrows' => FALSE,
  'autoplay' => TRUE,
  'vertical' => TRUE,
  'draggable' => FALSE,
);

// 6.A.
// Build the slick, note key 0 so to mark the thumbnail asNavFor with key 1.
$slick[0] = array(
  '#theme' => 'slick',
  '#items' => $items,
  '#settings' => $settings,
  '#options' => $options,
  // Attach the Slick library, see slick_attach() for more options.
  // At D8, #attached is obligatory to avoid issue with caching.
  '#attached' => slick_attach($attach),
);

// Optionally build an asNavFor with $slick[1], and both should be passed to
// theme_slick_wrapper(), otherwise a single theme_slick() will do.
// See slick_fields, or slick_views sub-modules for asNavFor samples.
// All is set, render the Slick.
print render($slick);

// 6.B.
// Or alternatively, use slick_build() where the parameters are as described
// above:
$slick = slick_build($items, $options, $settings, $attach, $id);
// All is set, render the Slick.
print render($slick);

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
 * Registers Slick dot skins.
 *
 * The provided dot skins will be available at sub-module interfaces.
 * A skin dot named 'hop' will have a class 'slick-dots--hop' for the UL.
 *
 * The array is similar to the hook_slick_skins_info().
 */
function hook_slick_dots_info() {
  // Create an array of dot skins.
}

/**
 * Registers Slick arrow skins.
 *
 * The provided arrow skins will be available at sub-module interfaces.
 * A skin arrow named 'slit' will have a class 'slick__arrow--slit' for the NAV.
 *
 * The array is similar to the hook_slick_skins_info().
 */
function hook_slick_arrows_info() {
  // Create an array of arrow skins.
}

/**
 * Alter Slick skins.
 *
 * This function lives in a module file, not my_module.slick.inc.
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
 * This function lives in a module file, not my_module.slick.inc.
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
