/**
 * @file
 * Slick loading file.
 */

(function ($, Drupal, drupalSettings) {

  "use strict";

  Drupal.behaviors.slick = {
    attach: function(context) {
      $(".slick:not(.unslick)", context).once("slick").each(function() {
        var t = $(this),
          configs = t.data("slick") || {};

        t.slick(configs);
      });
    }
  };

})(jQuery, Drupal, drupalSettings);
