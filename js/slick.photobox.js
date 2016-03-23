/**
 * @file
 * Provides Photobox integration for Image and Media fields.
 *
 * @todo move to Blazy for re-usability across Blazy, GridStack, Mason, Slick.
 */

(function ($, Drupal) {

  'use strict';

  Drupal.behaviors.slickPhotobox = {
    attach: function (context) {
      $('.slick--photobox', context).once('slick-photobox').each(function () {
        $(this).photobox('.slick__photobox', {thumbAttr: 'data-thumb'});
      });
    }
  };

}(jQuery, Drupal));
