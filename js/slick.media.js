/**
 * @file
 * Provides Media module integration.
 */

(function ($) {

  "use strict";

  Drupal.behaviors.slickMedia = {
    attach: function (context, settings) {

      $('.media--switch--player', context).once('slick-media', function () {
        var t = $(this),
          $slider = t.closest('.slick__slider'),
          iframe = t.find('iframe'),
          newIframe = iframe.clone(),
          media = newIframe.data('media'),
          url = newIframe.data('lazy');

        // Remove iframe to avoid browser requesting them till clicked.
        iframe.remove();

        t.on('click.media-play', '.media-icon--play', function (e) {
          var p = $(this);

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
          t.removeClass('is-playing');

          // Clean up any pause marker at slider container.
          $('.is-paused').removeClass('is-paused');

          // Last, pause the slide, for just in case autoplay is on, and
          // pauseOnHover is disabled, and then trigger autoplay.
          $slider.addClass('is-paused').slick('slickPause');

          t.addClass('is-playing').append(newIframe);
          newIframe.attr('src', url);

          return false;
        });
        // Closes the video.
        t.on('click.media-close', '.media-icon--close', function (e) {
          t.removeClass('is-playing').find('iframe').remove();
          $('.is-paused').removeClass('is-paused');
          return false;
        });

        // Turns off any video if any change to the slider.
        $slider.on('afterChange', function (e, slick, currentSlide) {
          $slider.find('.is-playing .media-icon--close').trigger('click.media-close');
        });
      });
    }
  };

})(jQuery);
