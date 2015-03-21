/**
 * @file
 * Provides Photobox integration for Image and Media fields.
 */

(function ($, Drupal) {

  "use strict";

  Drupal.behaviors.slickPhotobox = {
    attach: function (context, settings) {
      $('.slick', context).once('slick-photobox', function () {
        $(this).photobox('.slick__slide:not(.slick-cloned) .slick__photobox');
      });
    }
  };

}(jQuery, Drupal));
