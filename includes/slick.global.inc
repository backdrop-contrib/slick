<?php
/**
 * @file
 * Global functions across Slick field formatters and Views.
 */

/**
 * Returns items as a grid display.
 */
function slick_build_grids(array $build, &$settings) {
  $grids = array();
  if (!empty($settings['unslick'])) {
    // $settings['count'] = 1;
    $settings['current_display'] = 'main';
    $settings['current_item'] = 'grid';
    $settings['slick_classes'] = FALSE;

    // Displays all items if unslick is enforced such as plain grid to lightbox.
    $grids[0] = array(
      '#theme' => 'slick_grid',
      '#items' => $build,
      '#delta' => 0,
      '#settings' => array(
        'count' => 2,
        'skin' => $settings['skin'],
        'unslick' => !empty($settings['unslick']),
      ),
    );
  }
  else {
    // Otherwise do chunks to have a grid carousel.
    if ($visible_slides = $settings['visible_slides']) {
      $grid_items = array_chunk($build, $visible_slides, $settings['preserve_keys']);
      foreach ($grid_items as $delta => $grid_item) {
        $grids[] = array(
          '#theme' => 'slick_grid',
          '#items' => $grid_item,
          '#delta' => $delta,
          '#settings' => array(
            'count' => count($grid_items),
            'skin' => $settings['skin'],
          ),
        );
      }
    }
  }

  return $grids;
}
