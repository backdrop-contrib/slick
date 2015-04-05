<?php
/**
 * @file
 * Slick style plugin for the Views module.
 */

/**
 * Implements a style type plugin for the Views module.
 */
class SlickViewsPluginStyleSlick extends views_plugin_style {

  /**
   * Set default options.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options += array(
      'optionset' => array('default' => 'default'),
      'optionset_thumbnail' => array('default' => ''),
      'skin' => array('default' => ''),
      'skin_thumbnail' => array('default' => ''),
      'skin_arrows' => array('default' => ''),
      'skin_dots' => array('default' => ''),
      'asnavfor_main' => array('default' => ''),
      'asnavfor_thumbnail' => array('default' => ''),
      'asnavfor_auto' => array('default' => FALSE),
      'thumbnail_caption' => array('default' => ''),
      'grid' => array('default' => ''),
      'grid_medium' => array('default' => ''),
      'grid_small' => array('default' => ''),
      'visible_slides' => array('default' => ''),
      'preserve_keys' => array('default' => FALSE),
      'slide_field_wrapper' => array('default' => FALSE),
      'slide_title' => array('default' => ''),
      'slide_image' => array('default' => ''),
      'slide_thumbnail' => array('default' => ''),
      'slide_overlay' => array('default' => ''),
      'slide_link' => array('default' => ''),
      'slide_layout' => array('default' => ''),
      'slide_caption' => array('default' => array()),
      'override' => array('default' => FALSE),
      'overridables' => array('default' => array()),
      'id' => array('default' => ''),
    );

    return $options;
  }

  /**
   * Show a form to edit the style options.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    ctools_form_include($form_state, 'slick.admin', 'slick');

    $elements = array();
    $settings = $this->options;
    $skins = slick_skins_options();
    $optionsets = slick_optionset_options();
    $is_tooltip = array('class' => array('is-tooltip'));

    $elements['optionset'] = array(
      '#title' => t('Option set main'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#default_value' => $settings['optionset'],
      '#description' => t('Manage optionsets at <a href="@link">Slick carousel admin page</a>.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => $is_tooltip,
    );

    $elements['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin main'),
      '#options' => $skins,
      '#default_value' => $settings['skin'],
      '#empty_option' => t('- None -'),
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself.'),
      '#attributes' => $is_tooltip,
    );

    $elements['optionset_thumbnail'] = array(
      '#title' => t('Option set thumbnail'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['optionset_thumbnail'],
      '#description' => t('If provided, asNavFor aka thumbnail navigation applies. Provide each optionset a unique asNavFor class at <a href="@link" target="_blank">Slick carousel admin page</a>, or use the provided ID where Slick thumbnail navigation ID is suffixed with "-thumbnail". Leave empty to not use thumbnail navigation.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => $is_tooltip,
    );

    $elements['skin_thumbnail'] = array(
      '#type' => 'select',
      '#title' => t('Skin thumbnail'),
      '#options' => $skins,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['skin_thumbnail'],
      '#description' => t('Thumbnail navigation skin. Leave empty to not use thumbnail navigation.'),
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
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the main display, e.g.: #slick-main-thumbnail or .slick--nav. This should address the thumbnail display class or ID attributes. Slick thumbnail navigation ID is suffixed with "-thumbnail".'),
      '#attributes' => $is_tooltip,
    );

    $elements['asnavfor_thumbnail'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor thumbnail'),
      '#default_value' => $settings['asnavfor_thumbnail'],
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the thumbnail display, e.g.: #slick-main or .slick--for. This should address the main display class or ID attributes. Also provide thumbnail field below.'),
      '#attributes' => $is_tooltip,
    );

    $elements['asnavfor_auto'] = array(
      '#title' => t('asNavFor auto selectors'),
      '#type' => 'checkbox',
      '#default_value' => $settings['asnavfor_auto'],
      '#field_suffix' => '',
      '#title_display' => 'before',
      '#description' => t('If checked, asNavFor selectors will be determined by the current view ID selector automatically. Check if unsure with selectors.'),
      '#attributes' => $is_tooltip,
    );

    $elements['thumbnail_caption'] = array(
      '#type' => 'select',
      '#title' => t('Thumbnail caption'),
      '#default_value' => $settings['thumbnail_caption'],
      '#options' => array(),
      '#empty_option' => t('- None -'),
      '#description' => t('Thumbnail caption maybe just titles.'),
      '#attributes' => $is_tooltip,
      '#access' => FALSE,
    );

    $grid_options = drupal_map_assoc(range(1, 12));
    $elements['grid'] = array(
      '#type' => 'select',
      '#title' => t('Grid large'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid'],
      '#description' => t('The amount of block grid columns for large monitors 64.063em - 90em. Use skin Grid for starter. This module feature has been around before the Slick library officiallly supported it. Leave empty to use the library grid system instead.'),
      '#attributes' => $is_tooltip,
    );

    $elements['grid_medium'] = array(
      '#type' => 'select',
      '#title' => t('Grid medium'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid_medium'],
      '#description' => t('The amount of block grid columns for medium devices 40.063em - 64em.'),
      '#attributes' => $is_tooltip,
    );

    $elements['grid_small'] = array(
      '#type' => 'select',
      '#title' => t('Grid small'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid_small'],
      '#description' => t('The amount of block grid columns for small devices 0 - 40em.'),
      '#attributes' => $is_tooltip,
    );

    $elements['visible_slides'] = array(
      '#type' => 'select',
      '#title' => t('Visible slides'),
      '#options' => drupal_map_assoc(range(1, 32)),
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['visible_slides'],
      '#description' => t('How many items per slide displayed at a time related to this grid.'),
      '#attributes' => $is_tooltip,
    );

    $elements['preserve_keys'] = array(
      '#title' => t('Preserve keys'),
      '#type' => 'checkbox',
      '#default_value' => $settings['preserve_keys'],
      '#description' => t('If checked, keys will be preserved. Default is FALSE which will reindex the grid chunk numerically'),
      '#attributes' => $is_tooltip,
      '#field_suffix' => '.',
      '#title_display' => 'before',
    );

    $elements['slide_field_wrapper'] = array(
      '#title' => t('Provide Slick field wrapper'),
      '#type' => 'checkbox',
      '#default_value' => $settings['slide_field_wrapper'],
      '#description' => t('If checked, Slick will provides Slick markups for the following fields.'),
      '#attributes' => $is_tooltip,
      '#field_suffix' => '',
      '#title_display' => 'before',
    );

    $image_options = array();
    $caption_options = array();
    $layout_options = array();
    $link_options = array();
    $title_options = array();
    $thumbnail_caption_options = array();

    $fields = $this->view->display_handler->get_handlers('field');
    foreach ($fields as $field => $handler) {
      if (isset($handler->field_info)) {
        $type = $handler->field_info['type'];

        switch ($type) {
          case 'file':
          case 'image':
            $image_options[$field] = $handler->ui_name();
            break;

          case 'list_text':
            $layout_options[$field] = $handler->ui_name();
            break;

          case 'text':
          case 'text_long':
          case 'text_with_summary':
          case 'link_field':
            $link_options[$field] = $handler->ui_name();
            $title_options[$field] = $handler->ui_name();
            if ($type != 'link_field') {
              $thumbnail_caption_options[$field] = $handler->ui_name();
            }
            break;
        }
      }

      // Content: title is not really a field, unless title.module installed.
      if ($handler->field == 'title') {
        $title_options[$field] = $handler->ui_name();
        $thumbnail_caption_options[$field] = $handler->ui_name();
      }

      if ($handler->field == 'view_node') {
        $link_options[$field] = $handler->ui_name();
      }

      // Caption can be anything to get custom works going.
      $caption_options[$field] = $handler->ui_name();
    }

    if ($thumbnail_caption_options) {
      $elements['thumbnail_caption']['#options'] = $thumbnail_caption_options;
    }

    // Simple uniform layout.
    $layout_options = $layout_options ? $layout_options : slick_layouts();
    $elements['slide_layout'] = array(
      '#title' => t('Slide layout'),
      '#type' => 'select',
      '#default_value' => $settings['slide_layout'],
      '#options' => $layout_options,
      '#empty_option' => t('- None -'),
      '#description' => t('Be sure to choose "key", not "Default" under Formatter. If you provide a dedicate list text field, it will affect individual slide, otherwise it affects the entire slides within the current slick uniformly. Leave empty to DIY. Or use field formatter to allow more control with a dedicated slide layout field.'),
      '#attributes' => $is_tooltip,
      '#prefix' => '<h3 class="form--slick__title">' . t('Fields') . '</h3>',
    );

    // Title field.
    $elements['slide_title'] = array(
      '#title' => t('Slide title'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $title_options,
      '#default_value' => $settings['slide_title'],
      '#description' => t('If provided, it will bre wrapped with H2 and class .slide__title.'),
      '#attributes' => $is_tooltip,
    );

    // Main image field.
    $elements['slide_image'] = array(
      '#title' => t('Main image field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_image'],
      '#description' => t('Main image'),
      '#attributes' => $is_tooltip,
    );

    // Thumbnail field relevant to asNavFor.
    $elements['slide_thumbnail'] = array(
      '#title' => t('Thumbnail image'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_thumbnail'],
      '#description' => t("Oly needed if you provide <em>Option set thumbnail</em> to have thumbnail navigation. Make sure the size is smaller than the main image. It may be the same field as the main image, only different instance/ variant dedicated for thumbnails. Leave empty if you don't use thumbnail pager."),
      '#attributes' => $is_tooltip,
    );

    // Field collection overlay.
    $elements['slide_overlay'] = array(
      '#title' => t('Overlay image/video'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_overlay'],
      '#description' => t('If audio/video, make sure the display is not image display.'),
      '#attributes' => $is_tooltip,
    );

    // Link field.
    $elements['slide_link'] = array(
      '#title' => t('Link'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $link_options,
      '#default_value' => $settings['slide_link'],
      '#description' => t('Link to content: Read more, View Case Study, etc, wrapped with class .slide__link.'),
      '#attributes' => $is_tooltip,
    );

    // Caption fields.
    $slide_captions = is_array($settings['slide_caption']) ? array_values($settings['slide_caption']) : $settings['slide_caption'];

    $elements['slide_caption'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Caption fields'),
      '#description' => t("Select fields for the caption. This can also be set manually by adding the '.slide__caption' class to a field."),
      '#options' => $caption_options,
      '#default_value' => $slide_captions,
      '#attributes' => $is_tooltip,
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($elements['slide_caption']['#options'] as $key => $element) {
      $elements['slide_caption'][$key]['#field_suffix'] = '';
      $elements['slide_caption'][$key]['#title_display'] = 'before';
    }

    $elements['id'] = array(
      '#type' => 'textfield',
      '#title' => t('Element ID'),
      '#description' => t("Manually define the Slick carousel container ID attribute <em>Ensure you don't display similar ID elements on the same page</em>. If you provide Option set thumbnail and asNavFor, slick thumbnail navigation ID is suffixed with '-thumbnail'."),
      '#size' => 40,
      '#maxlength' => 255,
      '#default_value' => $settings['id'],
      '#attributes' => $is_tooltip,
      '#field_prefix' => '#',
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

    $form['slick'] = array(
      '#type' => 'fieldset',
      '#title' => t('Slick carousel'),
      '#attributes' => array('class' => array('form--slick form--compact form--field has-tooltip')),
      '#description' => t('Leave anything empty/unchecked, except Optionset and Skin, if working with custom markups. <br /><small>Otherwise only relevant markups are added like Field formatter ones, and you have to add supported fields to appear here. Views preview works with jQuery > 1.7.</small>'),
    );

    $no_wrappers = array(
      'optionset',
      'skin',
      'skin_arrows',
      'skin_dots',
      'id',
      'asnavfor_main',
      'grid',
      'grid_medium',
      'grid_small',
      'visible_slides',
      'preserve_keys',
      'slide_field_wrapper',
      'override',
      'overridables',
    );

    $grids = array(
      'grid_medium',
      'grid_small',
      'visible_slides',
      'preserve_keys',
    );

    foreach ($elements as $key => $element) {
      $form['slick'][$key] = $element;
      if (!in_array($key, $no_wrappers)) {
        $form['slick'][$key]['#states'] = array(
          'visible' => array(
            ':input[name*="[slide_field_wrapper]"]' => array('checked' => TRUE),
          ),
        );
      }

      if (in_array($key, array('thumbnail_caption', 'asnavfor_auto'))) {
        $form['slick'][$key]['#states'] = array(
          'invisible' => array(
            array('select[name*="[optionset_thumbnail]"]' => array('value' => '')),
            array(':input[name*="[slide_field_wrapper]"]' => array('checked' => FALSE)),
          ),
        );
      }

      if (in_array($key, array('asnavfor_main', 'asnavfor_thumbnail'))) {
        $form['slick'][$key]['#states'] = array(
          'invisible' => array(
            array('select[name*="[optionset_thumbnail]"]' => array('value' => '')),
            array(':input[name*="[slide_field_wrapper]"]' => array('checked' => FALSE)),
            array(':input[name*="[asnavfor_auto]"]' => array('checked' => TRUE)),
          ),
        );
      }

      if (in_array($key, $grids)) {
        $form['slick'][$key]['#states'] = array(
          'invisible' => array(
            array('select[name$="[grid]"]' => array('value' => '')),
          ),
        );
      }

      if ($key == 'overridables') {
        $form[$key]['#states'] = array(
          'visible' => array(
            ':input[name$="[override]"]' => array('checked' => TRUE),
          ),
        );
      }
    }

    $form['#attached']['library'][] = array('slick', 'slick.admin');
  }

  /**
   * Performs some cleanup tasks on the options array before saving it.
   */
  public function options_submit(&$form, &$form_state) {
    $options = &$form_state['values']['style_options'];

    // In some cases, namely when the style settings are overriden for this
    // display, the slick options aren't in a sub array. No idea why. But this
    // prevents a fatal error.
    if (!empty($options['slick'])) {
      // Pull the fieldset values one level up.
      $options += $options['slick'];
      unset($options['slick']);
    }
  }

  /**
   * Render the display in this style.
   */
  public function render() {

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $keys = array_keys($this->view->field);

    $build = array();
    foreach ($sets as $rows) {
      if (!empty($this->options['slide_field_wrapper'])) {
        foreach ($rows as $index => $row) {
          // Add main image field if so configured.
          if (($slide_image = $this->options['slide_image']) !== NULL) {
            $rows[$index]->image = $this->get_field($index, $slide_image);

            // Add thumbnail navigation if so configured.
            if (($slide_thumbnail = $this->options['slide_thumbnail']) !== NULL) {
              $rows[$index]->thumbnail = $this->get_field($index, $slide_thumbnail);
            }

            // Allows text-only thumbnail navigation, like regular text tabs.
            if (isset($this->options['thumbnail_caption']) && ($thumbnail_caption = $this->options['thumbnail_caption']) !== NULL) {
              $rows[$index]->thumbnail_caption = $this->get_field($index, $thumbnail_caption);
            }
          }

          // Add caption field if so configured.
          if ($slide_captions = $this->options['slide_caption']) {
            $slide_captions = is_array($slide_captions) ? array_filter($slide_captions) : (array) $slide_captions;

            $caption_items = array();
            foreach ($slide_captions as $key => $slide_caption) {
              $slide_caption_rendered = $this->get_field($index, $slide_caption);
              if (empty($slide_caption_rendered)) {
                continue;
              }
              if (in_array($slide_caption, array_values($keys))) {
                $caption_items[$key]['#markup'] = filter_xss_admin($slide_caption_rendered);
              }
            }
            if ($caption_items) {
              $rows[$index]->caption = $caption_items;
            }
          }

          // Individual caption fields, if any is provided.
          // Add overlay field if so configured.
          if (($slide_overlay = $this->options['slide_overlay']) !== NULL) {
            $rows[$index]->overlay = $this->get_field($index, $slide_overlay);
          }

          // Add title field if so configured.
          if (($slide_title = $this->options['slide_title']) !== NULL) {
            $rows[$index]->title = $this->get_field($index, $slide_title);
          }

          // Add link field if so configured.
          if (($slide_link = $this->options['slide_link']) !== NULL) {
            $rows[$index]->link = $this->get_field($index, $slide_link);
          }

          // Add layout field if so configured.
          if (($slide_layout = $this->options['slide_layout']) !== NULL) {
            // Slide layout may be a list field, or builtin layout options.
            if (strpos($slide_layout, 'field_') !== FALSE) {
              $rows[$index]->layout = $this->get_field($index, $slide_layout);
            }
            else {
              $rows[$index]->layout = $slide_layout;
            }
          }
        }
      }

      // Build the slick.
      $build = array(
        '#theme' => $this->theme_functions(),
        '#view' => $this->view,
        '#options' => $this->options,
        '#rows' => $rows,
      );
    }

    return $build;
  }

}
