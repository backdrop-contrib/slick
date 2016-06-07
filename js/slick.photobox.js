/**
 * @file
 * Provides Photobox integration for Image and Media fields.
 */

(function ($, Drupal) {

  "use strict";

  Drupal.behaviors.slickPhotobox = {
    attach: function (context) {
      $(".slick--photobox", context).once("slick-photobox", function () {
        $(this).photobox(".slick__photobox", {thumb: '> [data-thumb]', thumbAttr: "data-thumb"});
      });
    }
  };

}(jQuery, Drupal));
