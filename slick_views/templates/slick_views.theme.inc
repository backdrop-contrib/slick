<?php

/**
 * @file
 * Hooks and preprocess functions for the Slick Views module.
 */

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_slick_views(&$variables) {
  $element = array();

  // Prepare all data to be passed to theme_slick().
  if (!empty($variables['view'])) {
    $view = $variables['view'];
    $settings = $variables['options'];

    $items = array();
    $thumbs = array();

    $attach = array(
      'attach_media' => FALSE,
      'attach_colorbox' => FALSE,
      'attach_skin' => NULL,
      'attach_skin_thumbnail' => NULL,
    );

    foreach ($variables['rows'] as $id => $row) {
      // Sets row_index to get the rendered fields.
      $view->row_index = $id;
      $item = array(
        'item' => array(
          'type' => 'views',
          'view_name' => check_plain($view->name),
        ),
        'slide' => array(),
      );
      $thumb = array();

      if (!empty($settings['slide_field_wrapper'])) {
        $slide_image = $settings['slide_image'];

        // Main image, if so configured.
        if (isset($row->image) && !empty($row->image)) {
          $attach['attach_media'] = TRUE;
          $item['slide'] = $row->image;
          // @todo refine for asNavFor.
          if (isset($row->thumbnail) && !empty($row->thumbnail)) {
            $settings['thumbnail_style'] = TRUE;
            $thumb['slide'] = $row->thumbnail;
          }
        }

        // Caption fields.
        $item['caption'] = array();
        if (isset($row->caption) && !empty($row->caption)) {
          $item['caption']['data'] = $row->caption;
        }

        // Overlay, if so configured.
        if (isset($row->overlay) && !empty($row->overlay)) {
          $attach['attach_media'] = TRUE;
          $item['caption']['overlay'] = $row->overlay;
        }

        // Title, if so configured.
        if (isset($row->title) && !empty($row->title)) {
          $item['caption']['title'] = filter_xss_admin($row->title);
        }

        // Link, if so configured.
        if (isset($row->link) && !empty($row->link)) {
          $item['caption']['link'] = $row->link;
        }

        // Layout, if so configured.
        if (isset($row->layout) && !empty($row->layout)) {
          $item['caption']['layout'] = check_plain($row->layout);
        }
      }
      else {
        // @todo refine more.
        // Media is enabled to allow copying slick media template in replacement
        // for regular media_youtube and media_vimeo templates manually.
        $attach['attach_media'] = TRUE;
        $item['slide'] = $view->style_plugin->row_plugin->render($row);
      }

      $items[] = $item;

      if ($thumb) {
        $thumb['item']['type'] = 'views';
        $thumb['item']['view_name'] = $view->name;
        // @todo thumbnails may have caption when Slick resolves selector issue.
        $thumb['caption'] = array();
        $thumbs[] = $thumb;
      }
    }

    unset($view->row_index);

    // Add the settings.
    $settings['current_display'] = 'main';
    if ($settings['skin']) {
      $attach['attach_skin'] = $settings['skin'];
    }

    if (isset($settings['skin_thumbnail']) && $settings['skin_thumbnail']) {
      $attach['attach_skin_thumbnail'] = $settings['skin_thumbnail'];
    }

    // Build the Slick attributes.
    if ($settings['id']) {
      $settings['attributes']['id'] = $settings['id'];
    }

    $settings['attributes']['class'][] = drupal_clean_css_identifier('slick--view--' . $view->name);

    // Build the Slick grid.
    $grid = isset($settings['grid']) && $settings['grid'] ? $settings['grid'] : '';
    if ($grid) {
      $visible_slides = $settings['visible_slides'];
      // Only if we do have a reasonable amount of grid items.
      if ($visible_slides && count($variables['rows']) > $visible_slides) {
        $grid_items = array_chunk($items, $visible_slides);

        $grids = array();
        foreach ($grid_items as $delta => $grid_children) {
          $grids[] = array(
            '#theme' => 'slick_grid',
            '#items' => $grid_children,
            '#delta' => $delta,
            '#settings' => $settings,
          );
        }

        // Update the $items with grid chunks.
        $items = $grids;
      }
    }

    // Build the Slick main display.
    $element[0] = array(
      '#theme' => 'slick',
      '#items' => $items,
      '#settings' => $settings,
      '#attached' => slick_attach($attach),
    );

    // Build the Slick thumbnail display.
    if (isset($settings['optionset_thumbnail']) && $settings['optionset_thumbnail'] && $thumbs) {
      if ($settings['id']) {
        $settings['attributes']['id'] = $settings['id'] . '-thumbnail';
      }
      $settings['optionset'] = $settings['optionset_thumbnail'];
      $settings['current_display'] = 'thumbnail';
      $element[1] = array(
        '#theme' => 'slick',
        '#items' => $thumbs,
        '#settings' => $settings,
        '#attached' => array(),
      );
    }

    // Updates rows to contain Slick instances now.
    $variables['rows'] = $element;

    // Build the Slick wrapper attributes.
    $variables['attributes_array']['class'] = array('slick-wrapper');
    if (isset($settings['skin']) && $settings['skin']) {
      $variables['attributes_array']['class'][] = drupal_clean_css_identifier('slick-wrapper--' . $settings['skin']);
    }

    if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
      $variables['attributes_array']['class'][] = drupal_clean_css_identifier('slick-wrapper--asnavfor');
    }

    // Populate classes_array expected by Omega 3.
    // @todo drop it when Omega3/5 has a better preprocess like Omega 4.
    $variables['classes_array'] = $variables['attributes_array']['class'];
  }
}
