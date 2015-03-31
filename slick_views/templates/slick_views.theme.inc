<?php
/**
 * @file
 * Hooks and preprocess functions for the Slick Views module.
 */

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_slick_views(&$variables) {

  // Prepare all data to be passed to theme_slick().
  if ($view = $variables['view']) {
    $view_name = check_plain($view->name);
    $settings = $variables['options'];
    $element = array();
    $items = array();
    $thumbs = array();
    $attach = array(
      'attach_skin' => $settings['skin'],
      'attach_skin_arrows' => $settings['skin_arrows'],
      'attach_skin_dots' => $settings['skin_dots'],
    );

    foreach ($variables['rows'] as $id => $row) {
      // Sets row_index to get the rendered fields.
      $view->row_index = $id;
      $item = array(
        'item' => array(
          'type' => 'views',
          'view_name' => $view_name,
        ),
        'slide' => array(),
      );
      $thumb = array();

      if (!empty($settings['slide_field_wrapper'])) {
        // Main image, if so configured.
        if (isset($row->image) && !empty($row->image)) {
          $attach['attach_media'] = TRUE;
          $item['slide'] = $row->image;

          // Add thumbnail navigation if so configured.
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
          $item['caption']['link'] = filter_xss_admin($row->link);
        }

        // Layout, if so configured.
        if (isset($row->layout) && !empty($row->layout)) {
          $item['caption']['layout'] = check_plain($row->layout);
        }
      }
      else {
        // Media is enabled to allow copying slick media template in replacement
        // for regular media_youtube and media_vimeo templates manually.
        $attach['attach_media'] = TRUE;
        $item['slide'] = $view->style_plugin->row_plugin->render($row);
      }

      $items[] = $item;

      // Allows text-only thumbnail navigation, like regular tabs.
      if (isset($row->thumbnail_caption) && !empty($row->thumbnail_caption)) {
        $thumb['caption']['data']['#markup'] = filter_xss_admin($row->thumbnail_caption);
        if (!isset($thumb['slide'])) {
          $thumb['slide'] = array();
        }
      }

      if ($thumb) {
        $thumb['item']['type'] = 'views';
        $thumb['item']['view_name'] = $view_name;

        $thumbs[] = $thumb;
      }
    }

    unset($view->row_index);

    // Add the settings.
    $settings['current_display'] = 'main';
    $settings['count'] = count($items);
    $is_asnavfor = isset($settings['optionset_thumbnail']) && $settings['optionset_thumbnail'] && $thumbs;
    $is_asnavfor_auto = isset($settings['asnavfor_auto']) && $settings['asnavfor_auto'];

    // Build the Slick attributes.
    $slick_id = &drupal_static('slick_id', 0);
    $id = $settings['id'] ? $settings['id'] : drupal_clean_css_identifier("slick-views-{$view_name}-" . ++$slick_id);
    $thumbnail_id = $id . '-thumbnail';

    // Enforce ID for easy asNavFor selectors.
    $settings['attributes']['id'] = $id;
    $settings['content_attributes']['id'] = $id . '-slider';
    $settings['attributes']['class'][] = drupal_clean_css_identifier('slick--view--' . $view_name);

    if ($is_asnavfor) {
      $attach['attach_skin_thumbnail'] = $settings['skin_thumbnail'];

      if ($is_asnavfor_auto) {
        $settings['asnavfor_target'] = '#' . $thumbnail_id . '-slider';
      }
      elseif ($settings['asnavfor_main']) {
        $settings['asnavfor_target'] = $settings['asnavfor_main'];
      }
    }

    // Build the Slick grid.
    if (isset($settings['grid']) && $settings['grid']) {
      $visible_slides = $settings['visible_slides'];
      $preserve_keys = isset($settings['preserve_keys']) && $settings['preserve_keys'] ? TRUE : FALSE;
      // Only if we do have a reasonable amount of grid items.
      if ($visible_slides && count($variables['rows']) > $visible_slides) {
        $grid_items = array_chunk($items, $visible_slides, $preserve_keys);

        $grids = array();
        foreach ($grid_items as $delta => $grid_item) {
          $grids[] = array(
            '#theme' => 'slick_grid',
            '#items' => $grid_item,
            '#delta' => $delta,
            '#settings' => $settings,
          );
        }

        // Update the $items with grid chunks.
        $items = $grids;
      }
    }

    // Mimick the Slick wrapper attributes as we _can't actually use it till D8.
    $variables['attributes_array']['class'] = array('slick-wrapper');

    // Build the Slick main display.
    $element[0] = array(
      '#theme' => 'slick',
      '#items' => $items,
      '#settings' => $settings,
      '#attached' => slick_attach($attach),
    );

    // Overrides common options to re-use an optionset.
    if (isset($settings['override']) && $settings['override']) {
      $overridables = array();
      // Respect empty values, meaning FALSE.
      foreach ($settings['overridables'] as $key => $override) {
        $overridables[$key] = empty($override) ? FALSE : TRUE;
      }
      $element[0]['#options'] = $overridables;
    }

    // Build the Slick thumbnail display.
    if ($is_asnavfor) {
      $settings['optionset'] = $settings['optionset_thumbnail'];
      $settings['current_display'] = 'thumbnail';
      $settings['attributes']['id'] = $thumbnail_id;
      $settings['content_attributes']['id'] = $thumbnail_id . '-slider';
      $variables['attributes_array']['class'][] = 'slick-wrapper--asnavfor';

      if ($is_asnavfor_auto) {
        $settings['asnavfor_target'] = '#' . $id . '-slider';
      }
      elseif ($settings['asnavfor_thumbnail']) {
        $settings['asnavfor_target'] = $settings['asnavfor_thumbnail'];
      }

      $element[1] = array(
        '#theme' => 'slick',
        '#items' => $thumbs,
        '#settings' => $settings,
        '#attached' => array(),
      );
    }

    // Update rows to contain Slick instances now.
    $variables['rows'] = $element;

    if ($settings['skin']) {
      $variables['attributes_array']['class'][] = drupal_clean_css_identifier('slick-wrapper--' . $settings['skin']);
    }

    // Populate classes_array expected by Omega 3.
    $variables['classes_array'] = $variables['attributes_array']['class'];
  }
}
