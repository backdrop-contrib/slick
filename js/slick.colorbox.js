/**
 * @file
 * Provides Colorbox integration.
 */

(function ($, Drupal) {

  'use strict';

  Drupal.behaviors.slickColorbox = {
    attach: function (context) {
      $(context).on('cbox_open', function () {
        var $box = $.colorbox.element();
        var $slider = $box.closest('.slick__slider');

        if ($slider.length) {
          $slider.slick('slickPause');
        }
      });
    }
  };

}(jQuery, Drupal));
