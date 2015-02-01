/**
 * @file
 */
(function ($) {
  "use strict";
  Drupal.slick = Drupal.slick || {};

  Drupal.behaviors.slickMedia = {
    attach: function (context, settings) {

      var player = '.media--switch--player';

      $(player, context).once('slick-media', function () {
        var t = $(this),
          $slider = t.closest('.slick__slider');

        // Remove SRC attributes to avoid direct autoplay, if mistakenly enabled.
        t.find('iframe').attr('src', 'about:blank');

        t.on('click.media-play', '.media-icon--play', function (e) {
          var p = $(this),
            iframe = p.closest(player).find('iframe'),
            url = iframe.data('lazy'),
            media = iframe.data('media');

            // Soundcloud needs internet, fails on disconnected local.
            if (url === '') {
              return false;
            }
            // Force autoplay, if not provided, which should not.
            if (media.scheme === 'soundcloud') {
              if (url.indexOf('auto_play') < 0 || url.indexOf('auto_play') === false) {
                url = url.indexOf('?') < 0 ? url + '?auto_play=true' : url + '&amp;auto_play=true';
              }
            }
            else if (url.indexOf('autoplay') < 0 || url.indexOf('autoplay') === 0) {
              url = url.indexOf('?') < 0 ? url + '?autoplay=1' : url + '&amp;autoplay=1';
            }

          // First, reset any video to avoid multiple videos from playing.
          $(player).removeClass('is-playing').find('iframe').attr('src', 'about:blank');

          // Clean up any pause marker.
          $('.is-paused').removeClass('is-paused');

          // Last, pause the slide, for just in case autoplay is on, and
          // pauseOnHover is disabled, and then trigger autoplay.
          $slider.addClass('is-paused').slick('slickPause');

          t.closest(player).addClass('is-playing').find('iframe').attr('src', url);
          return false;
        })
        // Closes the video.
        .on('click.media-close', '.media-icon--close', function (e) {
          $(this).closest(player).removeClass('is-playing').find('iframe').attr('src', 'about:blank');
          $('.is-paused').removeClass('is-paused');
          return false;
        });

        // Turns off video if any button clicked.
        $slider
        .on('click.media-close-other', '.slick__arrow button, > button', function (e) {
          $slider.find('.media-icon--close').trigger('click.media-close');
        })
        .on('afterChange', function (e, slick, currentSlide) {
          $('.media-icon--close', $slider).trigger('click.media-close');
          // Drupal.slick.setCurrent($slider, currentSlide);
        });
        ;
      });
    }
  };
})(jQuery);