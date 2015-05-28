<?php
/**
 * @file
 * Misc functions for Slick fields admin.
 */

/**
 * Off-loaded hook_field_formatter_settings_form().
 */
function _slick_fields_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $path = drupal_get_path('module', 'slick_fields');
  $slick_module_path = drupal_get_path('module', 'slick');
  $display  = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $elements = array();

  $field_type = $field['type'];
  // $field empty at admin/structure/file-types/manage/image/file-display,
  // and this form is also loaded there via file_entity_file_formatter_info().
  if (empty($field)) {
    $field_type = $instance['entity_type'];
  }

  $is_picture  = function_exists('picture_get_mapping_options');
  $is_colorbox = function_exists('_colorbox_doheader');
  $is_photobox = function_exists('photobox_library');

  $layout_options  = array();
  $caption_options = array();
  $title_options   = array();
  $link_options    = array();
  $color_options   = array();
  $slide_classes   = array();
  $thumbnail_caption_options = array();
  $optionsets = slick_optionset_options();
  $is_tooltip = array('class' => array('is-tooltip'));

  $is_views_ui = '';
  if ($instance['entity_type'] == 'ctools') {
    $is_views_ui = '<h3 class="form--slick__title">' . t('Make sure to check "Use field template" under "Style settings" below, if using multiple-value field for the slides. Views preview works with jQuery > 1.7.') . '</h3>';
  }

  $elements['opening'] = array(
    '#markup' => '<div class="form--slick form--compact form--field has-tooltip">' . $is_views_ui,
  );

  $elements['optionset'] = array(
    '#title' => t('Option set main'),
    '#type' => 'select',
    '#options' => $optionsets,
    '#default_value' => $settings['optionset'],
    '#description' => t('Manage optionsets at <a href="@link" target="_blank">Slick carousel admin page</a>.', array('@link' => url('admin/config/media/slick'))),
    '#attributes' => $is_tooltip,
  );

  $skins = slick_skins_options();
  $elements['skin'] = array(
    '#type' => 'select',
    '#title' => t('Skin main'),
    '#options' => $skins,
    '#default_value' => $settings['skin'],
    '#empty_option' => t('- None -'),
    '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself. See main <a href="@url" target="_blank">README</a> for details on Skins. Leave empty to DIY, or use hook_slick_skins_info() to override one.', array('@url' => url($slick_module_path . '/README.txt'))),
    '#attributes' => $is_tooltip,
  );

  // Thumbnail navigation.
  $elements['optionset_thumbnail'] = array(
    '#title' => t('Option set thumbnail'),
    '#type' => 'select',
    '#options' => $optionsets,
    '#empty_option' => t('- None -'),
    '#default_value' => $settings['optionset_thumbnail'],
    '#description' => t('If provided, asNavFor aka thumbnail navigation applies. Provide each optionset a unique asNavFor class at <a href="@url" target="_blank">Slick carousel admin page</a>. Leave empty to not use thumbnail navigation.', array('@url' => url('admin/config/media/slick'))),
    '#attributes' => $is_tooltip,
  );

  $elements['skin_thumbnail'] = array(
    '#type' => 'select',
    '#title' => t('Skin thumbnail'),
    '#options' => $skins,
    '#default_value' => $settings['skin_thumbnail'],
    '#empty_option' => t('- None -'),
    '#description' => t('Thumbnail navigation skin. See main <a href="@url" target="_blank">README</a> for details on Skins. Leave empty to not use thumbnail navigation.', array('@url' => url($slick_module_path . '/README.txt'))),
    '#attributes' => $is_tooltip,
  );

  $elements['skin_arrows'] = array(
    '#type' => 'select',
    '#title' => t('Skin arrows'),
    '#options' => array(),
    '#default_value' => $settings['skin_arrows'],
    '#empty_option' => t('- None -'),
    '#description' => t('Use hook_slick_arrows_info() to add your own arrows skins, in the same format as hook_slick_skins_info().'),
    '#attributes' => $is_tooltip,
    '#access' => FALSE,
  );

  if ($arrows = slick_arrows_options()) {
    $elements['skin_arrows']['#options'] = $arrows;
    $elements['skin_arrows']['#access'] = TRUE;
  }

  $elements['skin_dots'] = array(
    '#type' => 'select',
    '#title' => t('Skin dots'),
    '#options' => array(),
    '#default_value' => $settings['skin_dots'],
    '#empty_option' => t('- None -'),
    '#description' => t('Use hook_slick_dots_info() to add your own dots skins, in the same format as hook_slick_skins_info().'),
    '#attributes' => $is_tooltip,
    '#access' => FALSE,
  );

  if ($dots = slick_dots_options()) {
    $elements['skin_dots']['#options'] = $dots;
    $elements['skin_dots']['#access'] = TRUE;
  }

  $elements['asnavfor_main'] = array(
    '#type' => 'textfield',
    '#title' => t('asNavFor main'),
    '#default_value' => $settings['asnavfor_main'],
    '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the main display, e.g.: #slick-node-page-field-images-1-thumbnail-slider, or .slick--nav. This should address the thumbnail display class or ID attributes.'),
    '#attributes' => array('class' => array('is-tooltip', 'js-expandable')),
    '#states' => array(
      'invisible' => array(
        array('select[name*="[optionset_thumbnail]"]' => array('value' => '')),
        array(':input[name*="[asnavfor_auto]"]' => array('checked' => TRUE)),
      ),
    ),
  );

  $elements['asnavfor_thumbnail'] = array(
    '#type' => 'textfield',
    '#title' => t('asNavFor thumbnail'),
    '#default_value' => $settings['asnavfor_thumbnail'],
    '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the thumbnail display, e.g.: #slick-node-page-field-images-1-slider, or .slick--for. This should address the main display class or ID attributes.'),
    '#attributes' => array('class' => array('is-tooltip', 'js-expandable')),
    '#states' => array(
      'invisible' => array(
        array('select[name*="[optionset_thumbnail]"]' => array('value' => '')),
        array(':input[name*="[asnavfor_auto]"]' => array('checked' => TRUE)),
      ),
    ),
  );

  $elements['asnavfor_auto'] = array(
    '#title' => t('asNavFor auto selectors'),
    '#type' => 'checkbox',
    '#default_value' => $settings['asnavfor_auto'],
    '#field_suffix' => '',
    '#title_display' => 'before',
    '#description' => t('If checked, asNavFor selectors will be determined by the current field ID selectors automatically. Check if unsure with selectors.'),
    '#attributes' => $is_tooltip,
    '#states' => array(
      'invisible' => array(
        'select[name*="[optionset_thumbnail]"]' => array('value' => ''),
      ),
    ),
  );

  $elements['thumbnail_caption'] = array(
    '#type' => 'select',
    '#title' => t('Thumbnail caption'),
    '#default_value' => $settings['thumbnail_caption'],
    '#options' => array(),
    '#empty_option' => t('- None -'),
    '#description' => t('Thumbnail caption maybe just titles.'),
    '#attributes' => $is_tooltip,
    '#states' => array(
      'invisible' => array(
        'select[name*="[optionset_thumbnail]"]' => array('value' => ''),
      ),
    ),
    '#access' => FALSE,
  );

  $image_styles = image_style_options(FALSE, PASS_THROUGH);
  $elements['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
    '#options' => $image_styles,
    '#description' => t('This is to override, or fallback as needed. This allows more variant per View mode with a single optionset.'),
    '#attributes' => $is_tooltip,
  );

  $elements['thumbnail_style'] = array(
    '#title' => t('Thumbnail style'),
    '#type' => 'select',
    '#default_value' => $settings['thumbnail_style'],
    '#empty_option' => t('- None -'),
    '#options' => $image_styles,
    '#description' => t('If <em>Option set thumbnail</em> provided, it is for asNavFor thumbnail navigation. Otherwise displayed when hovering over dots if <em>Dots with thumbnail</em> selected. Or to build arrows with thumbnails via the provided data-thumb attributes with your custom work. Leave empty to not use thumbnails.'),
    '#attributes' => $is_tooltip,
  );

  $elements['thumbnail_hover'] = array(
    '#title' => t('Dots with thumbnail'),
    '#type' => 'checkbox',
    '#default_value' => $settings['thumbnail_hover'],
    '#field_suffix' => '',
    '#title_display' => 'before',
    '#states' => array(
      'invisible' => array(
        'select[name*="[thumbnail_style]"]' => array('value' => ''),
      ),
    ),
    '#description' => t('Dependent on a skin, dots option enabled, and Thumbnail style. If checked, dots pager are kept, and thumbnail will be hidden and only visible on mouseover, default to min-width 120px. Alternative to asNavFor aka separate thumbnails as slider.'),
    '#attributes' => $is_tooltip,
  );

  $field_entity_type = $field_type;
  switch ($field_type) {
    // Core image field.
    case 'image':
      $layout_options = slick_layouts();
      break;

    // Media via file field.
    case 'file':
      $caption_options = _slick_fields_get_media_fields($instance);
      $layout_options  = _slick_fields_get_media_fields($instance, array('options_select'));
      $slide_classes   = _slick_fields_get_media_fields($instance, array('options_select', 'text_textfield'));
      $title_options   = _slick_fields_get_media_fields($instance, array(
        'link_field',
        'text_textfield',
        'text_textarea',
      ));
      $link_options = _slick_fields_get_media_fields($instance, array(
        'text_textfield',
        'link_field',
        'url_external',
      ));
      $thumbnail_caption_options = _slick_fields_get_media_fields($instance, array('text_textfield', 'text_textarea'));
      break;

    // This should go as a separate module, but alas.
    case 'field_collection':
      $field_entity_type = 'field_collection_item';
      $caption_options = _slick_fields_get_fc_fields($instance, array(
        'file',
        'image',
        'link_field',
        'list_text',
        'text_long',
        'text_with_summary',
        'taxonomy_term_reference',
        'text',
      ));

      $color_options   = _slick_fields_get_fc_fields($instance, array('text', 'color_field_rgb'));
      $layout_options  = _slick_fields_get_fc_fields($instance, array('list_text'));
      $image_options   = _slick_fields_get_fc_fields($instance, array('image', 'file'));
      $overlay_options = _slick_fields_get_fc_fields($instance, array(
        'image',
        'file',
        'atom_reference',
      ));
      $title_options = _slick_fields_get_fc_fields($instance, array(
        'link_field',
        'text',
        'text_long',
      ));
      $link_options = _slick_fields_get_fc_fields($instance, array(
        'text',
        'link_field',
        'url',
      ));
      $thumbnail_caption_options = _slick_fields_get_fc_fields($instance, array('text', 'text_long'));
      break;

    default:
      break;
  }

  // Layout to all supported fields.
  $elements['slide_layout'] = array(
    '#title' => t('Slide layout'),
    '#type' => 'select',
    '#default_value' => $settings['slide_layout'],
    '#options' => $layout_options,
    '#empty_option' => t('- None -'),
    '#description' => t('This affects the entire slides. Split half requires any skin Split. Leave empty to DIY.'),
    '#attributes' => $is_tooltip,
  );

  if (in_array($field_entity_type, array('field_collection_item', 'file'))) {
    $elements['slide_layout']['#prefix'] = '<h3 class="form--slick__title">' . t('Fields') . '</h3>';
    // Complex slide layout is provided by FC alone.
    // Skip media file field as it is the main/ background media already.
    if (in_array($field_entity_type, array('field_collection_item'))) {
      $elements['slide_image'] = array(
        '#title' => t('Main image field'),
        '#type' => 'select',
        '#options' => $image_options,
        '#empty_option' => t('None'),
        '#default_value' => $settings['slide_image'],
        '#description' => t('Main background image field'),
        '#attributes' => $is_tooltip,
      );

      // Field collection overlay.
      $elements['slide_overlay'] = array(
        '#title' => t('Overlay image/video field'),
        '#type' => 'select',
        '#options' => $overlay_options,
        '#empty_option' => t('- None -'),
        '#default_value' => $settings['slide_overlay'],
        '#description' => t('If audio/video, be sure the display is not image display. You can manage its display at its own entity View mode display, or below for the supported formatters: Image and Rendered file. Nested Slick carousel is supported. Supported fields: Image, Media, Atom reference'),
        '#attributes' => $is_tooltip,
      );

      // Field collection nested slicks, slicks within a slick.
      $elements['nested_slick'] = array(
        '#title' => t('Enable nested slicks'),
        '#type' => 'checkbox',
        '#default_value' => $settings['nested_slick'],
        '#description' => t('If checked, multiple overlay items will be rendered as nested slicks, otherwise only the first image/video/audio is displayed. Applied to Image and Rendered file formatters. Alternatively, use the Slick carousel formatter for this particular overlay field at its own View mode display to build similar nested slick, and leave this unchecked.'),
        '#attributes' => $is_tooltip,
        '#field_suffix' => '',
        '#title_display' => 'before',
        '#states' => array(
          'invisible' => array(
            'select[name*="[slide_overlay]"]' => array('value' => ''),
          ),
        ),
      );

      $elements['nested_style'] = array(
        '#title' => t('Nested image style'),
        '#type' => 'select',
        '#default_value' => $settings['nested_style'],
        '#empty_option' => t('- None -'),
        '#options' => $image_styles,
        '#description' => t('Choose the image style for the nested slicks to get them uniformly.'),
        '#attributes' => $is_tooltip,
        '#states' => array(
          'invisible' => array(
            array('select[name*="[slide_overlay]"]' => array('value' => '')),
            array(':input[name*="[nested_slick]"]' => array('checked' => FALSE)),
          ),
        ),
      );

      $elements['nested_optionset'] = array(
        '#title' => t('Nested optionset'),
        '#type' => 'select',
        '#options' => $optionsets,
        '#empty_option' => t('- None -'),
        '#default_value' => $settings['nested_optionset'],
        '#description' => t('Choose the optionset for the nested slicks.'),
        '#attributes' => $is_tooltip,
        '#states' => array(
          'invisible' => array(
            array('select[name*="[slide_overlay]"]' => array('value' => '')),
            array(':input[name*="[nested_slick]"]' => array('checked' => FALSE)),
          ),
        ),
      );
    }

    // Title field.
    $elements['slide_title'] = array(
      '#title' => t('Slide title field'),
      '#type' => 'select',
      '#empty_option' => t('None'),
      '#options' => $title_options,
      '#default_value' => $settings['slide_title'],
      '#description' => t('If provided, it will bre wrapped with H2 and class .slide__title.'),
      '#attributes' => $is_tooltip,
    );

    // Caption fields for FC and Media.
    $elements['slide_caption'] = array(
      '#title' => t('Caption fields'),
      '#type' => 'checkboxes',
      '#default_value' => $settings['slide_caption'],
      '#options' => $caption_options,
      '#description' => t('Enable any of the following fields as slide caption. These fields are treated and wrapped as captions. Make sure to make them visible at their relevant Manage display.'),
      '#attributes' => $is_tooltip,
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($elements['slide_caption']['#options'] as $key => $value) {
      $elements['slide_caption'][$key]['#field_suffix'] = '';
      $elements['slide_caption'][$key]['#title_display'] = 'before';
    }

    if ($field_entity_type == 'field_collection_item') {
      $elements['slide_caption']['#description'] .= t('And not choose the same field as main/ background image.');
    }

    if (empty($caption_options)) {
      $elements['slide_caption']['#description'] = t('No fields found for the captions. You need to add relevant fields for the captions.');
    }

    // Link field.
    $elements['slide_link'] = array(
      '#title' => t('Link field'),
      '#type' => 'select',
      '#empty_option' => t('None'),
      '#options' => $link_options,
      '#default_value' => $settings['slide_link'],
      '#description' => t('Link to content: Read more, View Case Study, etc, wrapped with class .slide__link.'),
      '#attributes' => $is_tooltip,
    );

    // Layout fields to make individual slide more variant.
    $elements['slide_layout']['#title'] = t('Slide layout field');
    $elements['slide_layout']['#description'] = t('Create a dedicated List (text - max number 1) field related to the caption placement with the following supported keys: top, right, bottom, left, center, center-top, etc. See <a href="@url" target="_blank">README</a> under "Slide layout" for more info. Split half requires any skin Split. Leave empty to DIY.', array('@url' => url($path . '/README.txt')));

    $view_mode_options = _slick_fields_view_modes($field_entity_type, TRUE);
    $elements['view_mode'] = array(
      '#type' => 'select',
      '#title' => t('View mode'),
      '#options' => $view_mode_options,
      '#default_value' => $settings['view_mode'],
      '#description' => t('The view mode is a specific display to contain the fieldable slides. Create more view modes using hook_entity_info_alter, or <a href="@url" target="_blank">entity_view_mode</a>.', array('@url' => '//drupal.org/project/entity_view_mode')),
      '#access' => count($view_mode_options) > 1,
      '#attributes' => $is_tooltip,
    );

    if ($field_entity_type == 'file') {
      $elements['view_mode']['#description'] .= t('For video, make sure to choose relevant video display, not as image.');

      // Slide class field.
      $elements['slide_classes'] = array(
        '#title' => t('Slide class field'),
        '#type' => 'select',
        '#empty_option' => t('None'),
        '#options' => $slide_classes,
        '#default_value' => $settings['slide_classes'],
        '#description' => t('If provided, individual slide will have this class, e.g.: to have different background with transparent images and skin Split.'),
        '#attributes' => $is_tooltip,
      );
    }
  }

  $elements['media_switch'] = array(
    '#title' => t('Media switcher'),
    '#type' => 'select',
    '#default_value' => $settings['media_switch'],
    '#options' => array(
      'iframe-switch' => t('Image to iframe switcher'),
    ),
    '#empty_option' => t('- None -'),
    '#description' => t('Supports Colorbox/ Photobox and Media iframe switcher.'),
    '#attributes' => $is_tooltip,
  );

  if (in_array($field_entity_type, array('field_collection_item', 'file'))) {
    $elements['media_switch']['#description'] .= t('<br />Iframe switcher: audio/video iframe is hidden below image overlay until toggled, otherwise iframe is always displayed, and draggable fails. Aspect ratio applies.');

    $elements['aspect_ratio'] = array(
      '#type' => 'select',
      '#title' => t('Aspect ratio'),
      // http://en.wikipedia.org/wiki/List_of_common_resolutions
      '#description' => t('Aspect ratio to get consistently responsive images and iframes within responsive layout, required if using media file to switch between iframe and overlay image, otherwise you have to do it properly. <a href="@dimensions" target="_blank">Image styles and video dimensions</a> must <a href="@follow" target="_blank">follow the ratio</a>, otherwise your images will be unexpedtedly resized. <a href="@link" target="_blank">Learn more</a>, or leave empty if you care not for aspect ratio, or prefer to DIY, etc. Check "Aspect ratio overlay only" below to apply to Media or FC overlay field only.', array(
        '@dimensions' => '//size43.com/jqueryVideoTool.html',
        '@follow' => '//en.wikipedia.org/wiki/Aspect_ratio_%28image%29',
        '@link' => '//www.smashingmagazine.com/2014/02/27/making-embedded-content-work-in-responsive-design/',
      )),
      '#options' => drupal_map_assoc(array('4:3', '16:9')),
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['aspect_ratio'],
      '#attributes' => $is_tooltip,
      '#states' => array(
        'visible' => array(
          'select[name*="[media_switch]"]' => array('value' => 'iframe-switch'),
        ),
      ),
    );

    if ($field_entity_type == 'field_collection_item') {
      $elements['media_switch']['#title'] = t('Media overlay switcher');
      $elements['media_switch']['#description'] .= t('<br />Both applied to overlay only.');
      $elements['aspect_ratio_overlay'] = array(
        '#title' => t('Aspect ratio overlay only'),
        '#type' => 'checkbox',
        '#default_value' => $settings['aspect_ratio_overlay'],
        '#field_suffix' => '',
        '#title_display' => 'before',
        '#states' => array(
          'invisible' => array(
            'select[name*="[aspect_ratio]"]' => array('value' => ''),
          ),
          'visible' => array(
            'select[name*="[media_switch]"]' => array('value' => 'iframe-switch'),
          ),
        ),
        '#description' => t('Apply aspect ratio to overlay media only, excluding main/background image.'),
        '#attributes' => $is_tooltip,
      );

      $elements['color_field'] = array(
        '#type' => 'select',
        '#title' => t('Color field'),
        '#options' => $color_options,
        '#default_value' => $settings['color_field'],
        '#description' => t('Color text field or <a href="@url" target="_blank">color_field</a> to colorize individual slide.', array('@url' => '//drupal.org/project/color_field')),
        '#attributes' => $is_tooltip,
        '#empty_option' => t('- None -'),
      );
    }
  }

  // Optional colorbox integration.
  if ($is_colorbox || $is_photobox) {

    // Re-use the same image style for both boxes.
    $elements['colorbox_style'] = array(
      '#title' => t('Colorbox image style'),
      '#type' => 'select',
      '#default_value' => $settings['colorbox_style'],
      '#empty_option' => t('None (original)'),
      '#options' => $image_styles,
      '#states' => array(
        'visible' => array(
          array('select[name*="[media_switch]"]' => array('value' => 'colorbox-switch')),
          array('select[name*="[media_switch]"]' => array('value' => 'photobox-switch')),
        ),
      ),
    );

    if ($is_colorbox) {
      $elements['media_switch']['#options']['colorbox-switch'] = t('Image to colorbox switcher');
      $elements['media_switch']['#description'] .= t('<br />Colorbox switcher: image to colorbox switch.');
    }

    if ($is_photobox) {
      $elements['media_switch']['#options']['photobox-switch'] = t('Image to photobox switcher');
      $elements['media_switch']['#description'] .= t('<br />Photobox switcher: image to photobox switch. Be sure to select Thumbnail style for the overlay thumbnails. Photobox is not compatible with infinite true + slidesToShow > 1, see Known Issues.');
      $elements['colorbox_style']['#title'] = t('Photobox image style');
    }
  }

  // Good old core image field.
  if ($field_entity_type == 'image') {
    unset($elements['media_switch']['#options']['iframe-switch']);
    if (!$is_colorbox && !$is_photobox) {
      $elements['media_switch']['#access'] = FALSE;
    }

    $slide_caption_options = $thumbnail_caption_options = array(
      'title_field' => t('Title'),
      'alt_field' => t('Alt'),
    );

    $elements['slide_caption'] = array(
      '#title' => t('Caption fields'),
      '#type' => 'checkboxes',
      '#default_value' => $settings['slide_caption'],
      '#options' => $slide_caption_options,
      '#description' => t('Captions will attempt to use Alt and Title attributes if enabled.'),
      '#attributes' => $is_tooltip,
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($elements['slide_caption']['#options'] as $key => $element) {
      $elements['slide_caption'][$key]['#field_suffix'] = '';
      $elements['slide_caption'][$key]['#title_display'] = 'before';
    }

    // If the image field doesn't have the Title field enabled, tell the user.
    if (isset($instance['settings']['title_field'])
      && $instance['settings']['title_field'] == FALSE
      || isset($instance['settings']['alt_field'])
      && $instance['settings']['alt_field'] == FALSE) {

      // Loosen up for Views UI.
      if ($instance['entity_type'] != 'ctools') {
        $elements['slide_caption']['#disabled'] = TRUE;
      }
      $elements['slide_caption']['#description'] = t('You need to <a href="@url" target="_blank">enable both title and alt fields</a> to use them as caption.', array(
        '@url' => url('admin/structure/types/manage/' . $instance['bundle'] . '/fields/' . $instance['field_name'],
            array(
              'fragment' => 'edit-instance-settings-title-field',
              'query' => array(
                'destination' => 'admin/structure/types/manage/' . $instance['bundle'] . '/display',
              ),
            )
          ),
      ));
    }
  }

  if ($thumbnail_caption_options) {
    $elements['thumbnail_caption']['#options'] = $thumbnail_caption_options;
    $elements['thumbnail_caption']['#access'] = TRUE;
  }

  // Picture module integration.
  // The Slick image_style with the picture mapping config.
  if ($is_picture) {
    $elements['picture'] = array(
      '#type' => 'checkbox',
      '#title' => t('Picture mapping'),
      '#description' => t('Picture mapping is only reasonable for large images. Note: The Slick lazyLoad is not supported with picture-enabled images. Slick only falicitates Picture to get in, and the image formatting is taken over by Picture.'),
      '#default_value' => $settings['picture'],
      '#field_suffix' => '',
      '#title_display' => 'before',
      '#attributes' => $is_tooltip,
    );

    $options = picture_get_mapping_options();
    $elements['picture_style'] = array(
      '#title' => t('Picture group'),
      '#states' => array(
        'visible' => array(
          ':input[name*="[picture]"]' => array('checked' => TRUE),
        ),
      ),
    );

    if (!empty($options)) {
      $elements['picture_style'] += array(
        '#type' => 'select',
        '#description' => t('Picture mapping for the main stage image.'),
        '#options' => $options,
        '#default_value' => $settings['picture_style'],
        '#attributes' => $is_tooltip,
      );
    }
    else {
      $elements['picture_style'] += array(
        '#type' => 'item',
        '#markup' => t('<a href="@url" target="_blank">No picture mappings</a> defined.', array('@url' => url('admin/config/media/picture'))),
      );
    }

    $elements['picture_fallback'] = array(
      '#title' => t('Fallback image style'),
      '#type' => 'select',
      '#default_value' => $settings['picture_fallback'],
      '#empty_option' => t('Automatic'),
      '#options' => $image_styles,
      '#states' => array(
        'visible' => array(
          ':input[name*="[picture]"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $elements['mousewheel'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable mousewheel'),
    '#description' => t('Make sure to download the <a href="@url" target="_blank">mousewheel</a> library, and it is available at <em>sites/.../libraries/mousewheel/jquery.mousewheel.min.js</em>.', array('@url' => '//github.com/brandonaaron/jquery-mousewheel')),
    '#default_value' => $settings['mousewheel'],
    '#field_suffix' => '',
    '#title_display' => 'before',
    '#attributes' => $is_tooltip,
  );

  $elements['override'] = array(
    '#title' => t('Override optionset'),
    '#type' => 'checkbox',
    '#default_value' => $settings['override'],
    '#description' => t('If checked, the following options will override the main optionset. Useful to re-use one optionset for several different displays.'),
    '#attributes' => $is_tooltip,
    '#field_suffix' => '',
    '#title_display' => 'before',
  );

  $overridable_options = slick_get_overridable_options();
  $overridable_values = is_array($settings['overridables']) ? array_values($settings['overridables']) : $settings['overridables'];
  $elements['overridables'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Overridable options'),
    '#description' => t("Override the main optionset to re-use one. Anything dictated here will override the current optionset. OFF means FALSE"),
    '#options' => $overridable_options,
    '#default_value' => $overridable_values,
    '#attributes' => $is_tooltip,
    '#states' => array(
      'visible' => array(
        ':input[name$="[override]"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Add empty suffix to style checkboxes like iOS.
  foreach ($elements['overridables']['#options'] as $key => $element) {
    $elements['overridables'][$key]['#field_suffix'] = '';
    $elements['overridables'][$key]['#title_display'] = 'before';
  }

  $elements['closing'] = array(
    '#markup' => '</div>',
  );

  if ($display['type'] == 'slick') {
    $elements['#attached']['library'][] = array('slick', 'slick.admin');
  }

  return $elements;
}

/**
 * Off-loaded hook_field_formatter_settings_summary().
 */
function _slick_fields_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();
  if ($display['type'] == 'slick') {
    $skins = slick_skins();

    if (isset($settings['skin'])) {
      $skin = $settings['skin'] ? $skins[$settings['skin']]['name'] : t('None');
      $summary[] = t('Skin: @skin', array('@skin' => $skin));
    }

    if (isset($settings['skin_thumbnail'])) {
      $skin_thumbnail = $settings['skin_thumbnail'] ? $skins[$settings['skin_thumbnail']]['name'] : t('None');
      $summary[] = t('Skin thumbnail: @skin_thumbnail', array('@skin_thumbnail' => $skin_thumbnail));
    }

    if (isset($settings['optionset'])) {
      $optionset = $settings['optionset'] ? $settings['optionset'] : t('Default');
      $summary[] = t('Optionset: @optionset', array('@optionset' => $optionset));
    }

    if (isset($settings['optionset_thumbnail'])) {
      $optionset_thumbnail = $settings['optionset_thumbnail'] ? $settings['optionset_thumbnail'] : t('None');
      $summary[] = t('Optionset thumbnail: @optionset_thumbnail', array('@optionset_thumbnail' => $optionset_thumbnail));
      $summary[] = isset($settings['thumbnail_caption']) && $settings['thumbnail_caption'] ? t('Yes') : t('None');

      if (isset($settings['asnavfor_auto']) && $settings['asnavfor_auto']) {
        $summary[] = t('asNavFor auto selectors: Yes');
      }
      else {
        if (isset($settings['asnavfor_main'])) {
          $asnavfor_main = $settings['asnavfor_main'] ? $settings['asnavfor_main'] : t('None');
          $summary[] = t('asNavFor main: @asnavfor_main', array('@asnavfor_main' => $asnavfor_main));
        }
        if (isset($settings['asnavfor_thumbnail'])) {
          $asnavfor_thumbnail = $settings['asnavfor_thumbnail'] ? $settings['asnavfor_thumbnail'] : t('None');
          $summary[] = t('asNavFor thumbnail: @asnavfor_thumbnail', array('@asnavfor_thumbnail' => $asnavfor_thumbnail));
        }
      }
    }

    if (isset($settings['image_style'])) {
      $image_style = $settings['image_style'] ? $settings['image_style'] : t('None (original image)');
      $summary[] = t('Image style: @image_style', array('@image_style' => $image_style));
    }

    if (!empty($settings['view_mode'])) {
      $summary[] = t('File view mode: @view_mode', array('@view_mode' => $settings['view_mode']));
    }

    if (isset($settings['slide_image'])) {
      $slide_image = $settings['slide_image'] ? $settings['slide_image'] : t('Self');
      $summary[] = t('Image: @slide_image', array('@slide_image' => $slide_image));
    }

    if (isset($settings['thumbnail_style'])) {
      $thumbnail_style = $settings['thumbnail_style'] ? $settings['thumbnail_style'] : t('None');
      $summary[] = t('Thumbnail: @thumbnail_style', array('@thumbnail_style' => $thumbnail_style));
      if (isset($settings['thumbnail_hover'])) {
        $thumbnail_hover = $settings['thumbnail_hover'] ? t('Yes') : t('None');
        $summary[] = t('Thumbnail hover: @thumbnail_hover', array('@thumbnail_hover' => $thumbnail_hover));
      }
    }

    if (isset($settings['slide_overlay'])) {
      $slide_overlay = $settings['slide_overlay'] ? $settings['slide_overlay'] : t('None');
      $summary[] = t('Overlay image/video: @slide_overlay', array('@slide_overlay' => $slide_overlay));

      if (isset($settings['nested_slick'])) {
        $nested_slick = $settings['nested_slick'] ? t('Yes') : t('No');
        $summary[] = t('Nested slicks: @nested_slick', array('@nested_slick' => $nested_slick));
      }
    }

    if (isset($settings['slide_layout'])) {
      $slide_layout = $settings['slide_layout'] ? $settings['slide_layout'] : t('None');
      $summary[] = t('Layout: @slide_layout', array('@slide_layout' => $slide_layout));
    }

    if (isset($settings['media_switch'])) {
      $media_switch = $settings['media_switch'] ? $settings['media_switch'] : t('None');
      $summary[] = t('Media switch: @media_switch', array('@media_switch' => $media_switch));
    }

    if (isset($settings['slide_title'])) {
      $slide_title = $settings['slide_title'] ? $settings['slide_title'] : t('None');
      $summary[] = t('Title: @slide_title', array('@slide_title' => $slide_title));
    }

    if (isset($settings['slide_link'])) {
      $slide_link = $settings['slide_link'] ? $settings['slide_link'] : t('None');
      $summary[] = t('Link: @slide_link', array('@slide_link' => $slide_link));
    }

    if (isset($settings['slide_classes'])) {
      $slide_classes = $settings['slide_classes'] ? $settings['slide_classes'] : t('None');
      $summary[] = t('Slide classes: @slide_classes', array('@slide_classes' => $slide_classes));
    }

    if (!empty($settings['slide_caption'])) {
      $captions = $settings['slide_caption'];
      if (is_array($settings['slide_caption'])) {
        $captions = array_filter(array_values($settings['slide_caption']));
        $captions = str_replace('_', ' ', $captions);
        $captions = implode(", ", $captions);
      }
      $summary[] = t('Caption: @captions', array('@captions' => $captions));
    }
    else {
      $summary[] = t('Caption: None');
    }

    if (isset($settings['color_field'])) {
      $color_field = $settings['color_field'] ? $settings['color_field'] : t('None');
      $summary[] = t('Color: @color_field', array('@color_field' => $color_field));
    }

    // Picture supports.
    if (isset($settings['picture'])) {
      $picture_style = $settings['picture_style'] ? $settings['picture_style'] : t('None (original image)');
      $summary[] = t('Picture style: @picture_style', array('@picture_style' => $picture_style));

      if (!empty($settings['picture_fallback'])) {
        $summary[] = t('Picture fallback: %picture_fallback', array('%picture_fallback' => $settings['picture_fallback']));
      }
    }

    if (isset($settings['mousewheel'])) {
      $mousewheel = $settings['mousewheel'] ? t('Yes') : t('No');
      $summary[] = t('Mousewheel: @mousewheel', array('@mousewheel' => $mousewheel));
    }

    if (isset($settings['override'])) {
      $override = $settings['override'] ? t('Yes') : t('No');
      $summary[] = t('Override optionset: @override', array('@override' => $override));
    }
  }

  return $summary;
}

/**
 * A helper function to return view modes for a form.
 *
 * @param string $entity_type
 *   The entity type to use with entity_get_info($entity_type) or
 *   entity_get_info($field['settings']['target_type']).
 * @param bool $exclude
 *   If TRUE, then exclude some view modes, such as: 'rss', 'search_index',
 *   'search_result', 'print', 'token'.
 *
 * @return array
 *   An array of available view modes, excluding some.
 */
function _slick_fields_view_modes($entity_type, $exclude = TRUE) {
  $view_mode_options = &drupal_static(__FUNCTION__);

  if (!isset($view_mode_options)) {
    $view_mode_options = array('default' => t('Default'));
    $view_mode_excludes = array(
      'rss',
      'search_index',
      'search_result',
      'print',
      'token',
      'preview',
      'wysiwyg',
    );

    $entity_info = entity_get_info($entity_type);
    if (!empty($entity_info['view modes'])) {
      foreach ($entity_info['view modes'] as $view_mode => $view_mode_settings) {
        if ($exclude && in_array($view_mode, $view_mode_excludes)) {
          continue;
        }
        $view_mode_options[$view_mode] = $view_mode_settings['label'];
      }
    }
  }

  return $view_mode_options;
}

/**
 * Gets a list of fields in the Field Collection item.
 */
function _slick_fields_get_fc_fields($instance, $field_types) {
  $fc_options = array();
  $fields = field_info_fields();
  $bundle = 'field_collection_item';
  $bundle_instance = $instance['field_name'];

  foreach ($fields as $name => $field) {
    $infos = field_info_instance($bundle, $name, $bundle_instance);
    if ($field_types == '_all') {
      $fc_options[$name] = $infos['label'];
    }
    else {
      if (in_array($bundle, array_keys($field['bundles']))
          && in_array($bundle_instance, $field['bundles'][$bundle])
          && in_array($field['type'], $field_types)) {
        $fc_options[$name] = $infos['label'];
      }
    }
  }

  return $fc_options;
}

/**
 * Gets a list of fields in the Media file field.
 */
function _slick_fields_get_media_fields($instance, $widget_types = NULL) {
  $media_options = array();
  $types = drupal_map_assoc(array('audio', 'image', 'video'));

  // Expose all media sub-fields, and merge them as available options.
  foreach ($types as $media_type) {
    $fields = field_info_instances('file', $media_type);
    foreach ($fields as $key => $media_field) {
      $type = $media_field['widget']['type'];
      if ($widget_types && in_array($type, $widget_types)) {
        $media_options[$key] = $media_field['label'];
      }
      else {
        $media_options[$key] = $media_field['label'];
      }
    }
  }

  return $media_options;
}
