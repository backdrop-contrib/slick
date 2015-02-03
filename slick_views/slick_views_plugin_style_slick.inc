<?php

/**
 * @file
 * slick style plugin for the Views module.
 */

/**
 * Implements a style type plugin for the Views module.
 */
class SlickViewsPluginStyleSlick extends views_plugin_style {

  /**
   * Set default options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options += array(
      'optionset' => array('default' => 'default'),
      'optionset_thumbnail' => array('default' => ''),
      'skin' => array('default' => ''),
      'skin_thumbnail' => array('default' => ''),
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
      'id' => array('default' => ''),
    );

    return $options;
  }

  /**
   * Show a form to edit the style options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    module_load_include('inc', 'slick', 'includes/slick.admin');

    $slick_module_path = drupal_get_path('module', 'slick');
    $element = array();
    $settings = $this->options;
    $skins = slick_skins(TRUE);
    $image_styles = image_style_options(FALSE, PASS_THROUGH);
    $optionsets = slick_optionset_options();

    $element['optionset'] = array(
      '#title' => t('Option set main'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#default_value' => $settings['optionset'],
      '#description' => t('Manage optionsets at <a href="@link">Slick carousel admin page</a>.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin main'),
      '#options' => $skins,
      '#default_value' => $settings['skin'],
      '#empty_option' => t('- None -'),
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['optionset_thumbnail'] = array(
      '#title' => t('Option set thumbnail'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['optionset_thumbnail'],
      '#description' => t('If provided, asNavFor aka thumbnail navigation applies. Provide each optionset a unique asNavFor class at <a href="@link" target="_blank">Slick carousel admin page</a>, or use the provided ID where Slick thumbnail navigation ID is suffixed with "-thumbnail". Leave empty to not use thumbnail navigation.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin_thumbnail'] = array(
      '#type' => 'select',
      '#title' => t('Skin thumbnail'),
      '#options' => $skins,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['skin_thumbnail'],
      '#description' => t('Thumbnail navigation skin. Leave empty to not use thumbnail navigation.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_main'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor main'),
      '#default_value' => $settings['asnavfor_main'],
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the main display, e.g.: #slick-main-thumbnail or .slick--nav. This should address the thumbnail display class or ID attributes. Slick thumbnail navigation ID is suffixed with "-thumbnail".'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_thumbnail'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor thumbnail'),
      '#default_value' => $settings['asnavfor_thumbnail'],
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the thumbnail display, e.g.: #slick-main or .slick--for. This should address the main display class or ID attributes. Also provide thumbnail field below.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_auto'] = array(
      '#title' => t('asNavFor auto selectors'),
      '#type' => 'checkbox',
      '#default_value' => $settings['asnavfor_auto'],
      '#field_suffix' => '',
      '#title_display' => 'before',
      '#description' => t('If checked, asNavFor selectors will be determined by the current view ID selector automatically. Check if unsure with selectors.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['thumbnail_caption'] = array(
      '#type' => 'select',
      '#title' => t('Thumbnail caption'),
      '#default_value' => $settings['thumbnail_caption'],
      '#options' => array(),
      '#empty_option' => t('- None -'),
      '#description' => t('Thumbnail caption maybe just titles.'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#access' => FALSE,
    );

    $grid_options = drupal_map_assoc(range(1, 12));
    $element['grid'] = array(
      '#type' => 'select',
      '#title' => t('Grid large'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid'],
      '#description' => t('The amount of block grid columns for large monitors 64.063em - 90em. Use skin Grid for starter, and plays with slidesToShow to have more grid combination.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['grid_medium'] = array(
      '#type' => 'select',
      '#title' => t('Grid medium'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid_medium'],
      '#description' => t('The amount of block grid columns for medium devices 40.063em - 64em.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['grid_small'] = array(
      '#type' => 'select',
      '#title' => t('Grid small'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['grid_small'],
      '#description' => t('The amount of block grid columns for small devices 0 - 40em.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['visible_slides'] = array(
      '#type' => 'select',
      '#title' => t('Visible slides'),
      '#options' => drupal_map_assoc(range(1, 32)),
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['visible_slides'],
      '#description' => t('How many items per slide displayed at a time related to this grid.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['preserve_keys'] = array(
      '#title' => t('Preserve keys'),
      '#type' => 'checkbox',
      '#default_value' => $settings['preserve_keys'],
      '#description' => t('If checked, keys will be preserved. Default is FALSE which will reindex the grid chunk numerically'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#field_suffix' => '.',
      '#title_display' => 'before',
    );

    $element['slide_field_wrapper'] = array(
      '#title' => t('Provide Slick field wrapper'),
      '#type' => 'checkbox',
      '#default_value' => $settings['slide_field_wrapper'],
      '#description' => t('If checked, Slick will provides Slick markups for the following fields.'),
      '#attributes' => array('class' => array('is-tooltip')),
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

      // Caption can be anything to get custom works going.
      $caption_options[$field] = $handler->ui_name();
    }

    if ($thumbnail_caption_options) {
      $element['thumbnail_caption']['#options'] = $thumbnail_caption_options;
    }

    // Simple uniform layout.
    $layout_options = $layout_options ? $layout_options : slick_layouts();
    $element['slide_layout'] = array(
      '#title' => t('Slide layout'),
      '#type' => 'select',
      '#default_value' => $settings['slide_layout'],
      '#options' => $layout_options,
      '#empty_option' => t('- None -'),
      '#description' => t('Be sure to choose "key", not "Default" under Formatter. If you provide a dedicate list text field, it will affect individual slide, otherwise it affects the entire slides within the current slick uniformly. Leave empty to DIY. Or use field formatter to allow more control with a dedicated slide layout field.'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#prefix' => '<h3 class="form--slick__title">' . t('Fields') . '</h3>',
    );

    // Title field.
    $element['slide_title'] = array(
      '#title' => t('Slide title'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $title_options,
      '#default_value' => $settings['slide_title'],
      '#description' => t('If provided, it will bre wrapped with H2 and class .slide__title.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Main image field.
    $element['slide_image'] = array(
      '#title' => t('Main image field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_image'],
      '#description' => t('Main image'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Thumbnail field relevant to asNavFor.
    $element['slide_thumbnail'] = array(
      '#title' => t('Thumbnail image'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_thumbnail'],
      '#description' => t("Oly needed if you provide <em>Option set thumbnail</em> to have thumbnail navigation. Make sure the size is smaller than the main image. It may be the same field as the main image, only different instance/ variant dedicated for thumbnails. Leave empty if you don't use thumbnail pager."),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Field collection overlay.
    $element['slide_overlay'] = array(
      '#title' => t('Overlay image/video'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_overlay'],
      '#description' => t('If audio/video, make sure the display is not image display.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Link field.
    $element['slide_link'] = array(
      '#title' => t('Link'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $link_options,
      '#default_value' => $settings['slide_link'],
      '#description' => t('Link to content: Read more, View Case Study, etc, wrapped with class .slide__link.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Caption fields.
    $slide_captions = is_array($settings['slide_caption']) ? array_values($settings['slide_caption']) : $settings['slide_caption'];

    $element['slide_caption'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Caption fields'),
      '#description' => t("Select fields for the caption. This can also be set manually by adding the '.slide__caption' class to a field."),
      '#options' => $caption_options,
      '#default_value' => $slide_captions,
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($element['slide_caption']['#options'] as $key => $value) {
      $element['slide_caption'][$key]['#field_suffix'] = '';
      $element['slide_caption'][$key]['#title_display'] = 'before';
    }

    $element['id'] = array(
      '#type' => 'textfield',
      '#title' => t('Element ID'),
      '#description' => t("Manually define the Slick carousel container ID attribute <em>Ensure you don't display similar ID elements on the same page</em>. If you provide Option set thumbnail and asNavFor, slick thumbnail navigation ID is suffixed with '-thumbnail'."),
      '#size' => 40,
      '#maxlength' => 255,
      '#default_value' => $settings['id'],
      '#attributes' => array('class' => array('is-tooltip')),
      '#field_prefix' => '#',
    );

    $form['slick'] = array(
      '#type' => 'fieldset',
      '#title' => t('Slick carousel'),
      '#attributes' => array('class' => array('form--slick form--compact form--field has-tooltip')),
      '#description' => t('Leave anything empty/unchecked, except Optionset and Skin, if working with custom markups. <br /><small>Otherwise only relevant markups are added like Field formatter ones, and you have to add supported fields to appear here. Views preview works with jQuery > 1.7.</small>'),
    );

    foreach ($element as $key => $item) {
      $form['slick'][$key] = $item;
      if (!in_array($key, array(
        'optionset',
        'skin',
        'slide_field_wrapper',
        'id',
        'asnavfor_main',
        'grid',
        'grid_medium',
        'grid_small',
        'visible_slides',
        'preserve_keys',
      ))) {
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
      $grids = array(
        'grid_medium',
        'grid_small',
        'visible_slides',
        'preserve_keys',
      );
      if (in_array($key, $grids)) {
        $form['slick'][$key]['#states'] = array(
          'invisible' => array(
            array('select[name$="[grid]"]' => array('value' => '')),
          ),
        );
      }
    }

    $form['#attached']['css'][] = $slick_module_path . '/css/admin/slick.admin--ui.css';
    $form['#attached']['css'][] = $slick_module_path . '/css/admin/slick.admin--ui--field.css';
    $form['#attached']['js'][] = $slick_module_path . '/js/slick.admin.ui.js';
  }

  /**
   * Performs some cleanup tasks on the options array before saving it.
   */
  function options_submit(&$form, &$form_state) {
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
  function render() {

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $keys = array_keys($this->view->field);

    $build = array();
    foreach ($sets as $rows) {
      if (!empty($this->options['slide_field_wrapper'])) {
        foreach ($rows as $index => $row) {
          // Add main image field if so configured.
          if (!empty($this->options['slide_image'])) {
            $slide_image = $this->options['slide_image'];
            $slide_thumbnail = $thumbnail_caption = '';
            if (!empty($this->options['slide_thumbnail'])) {
              $slide_thumbnail = $this->options['slide_thumbnail'];
            }

            if (isset($this->options['thumbnail_caption']) && !empty($this->options['thumbnail_caption'])) {
              $thumbnail_caption = $this->options['thumbnail_caption'];
            }

            $rows[$index]->image = $this->get_field($index, $slide_image);
            if ($slide_thumbnail) {
              $rows[$index]->thumbnail = $this->get_field($index, $slide_thumbnail);
              if ($thumbnail_caption) {
                $rows[$index]->thumbnail_caption = $this->get_field($index, $thumbnail_caption);
              }
            }
          }

          // Add caption field if so configured.
          if (!empty($this->options['slide_caption'])) {
            $slide_captions = $this->options['slide_caption'];
            $slide_captions = is_array($slide_captions) ? array_filter($slide_captions) : (array) $slide_captions;

            $caption_items = array();
            foreach ($slide_captions as $key => $slide_caption) {
              $slide_caption_rendered = $this->get_field($index, $slide_caption);
              if (empty($slide_caption_rendered)) {
                continue;
              }
              if (in_array($slide_caption, array_values($keys))) {
                $caption_items[$key]['#markup'] = $slide_caption_rendered;
              }
            }
            if ($caption_items) {
              $rows[$index]->caption = $caption_items;
            }
          }

          // Individual caption fields, if any is provided.
          // Add overlay field if so configured.
          if (!empty($this->options['slide_overlay'])) {
            $slide_overlay = $this->options['slide_overlay'];
            $rows[$index]->overlay = $this->get_field($index, $slide_overlay);
          }

          // Add title field if so configured.
          if (!empty($this->options['slide_title'])) {
            $slide_title = $this->options['slide_title'];
            $rows[$index]->title = $this->get_field($index, $slide_title);
          }

          // Add link field if so configured.
          if (!empty($this->options['slide_link'])) {
            $slide_link = $this->options['slide_link'];
            $rows[$index]->link = $this->get_field($index, $slide_link);
          }

          // Add layout field if so configured.
          if (!empty($this->options['slide_layout'])) {
            $slide_layout = $this->options['slide_layout'];
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
