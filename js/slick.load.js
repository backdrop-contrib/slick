/**
 * @file
 */
(function ($) {
  "use strict";

  Drupal.slick = Drupal.slick || {},
    Drupal.slick.callbacks = {};

  Drupal.behaviors.slick = {
    attach: function (context, settings) {

      $('.slick', context).once('slick', function () {
        var self = this,
          t = $(self),
          defaults = settings.slick || {},
          configs = t.data('config') || {},
          merged = $.extend({}, defaults, configs),
          index = $('.slick__slide:not(.slick-cloned)', self).index(),
          callbacks = Drupal.slick.runCallbacks(t, index) || {},
          globals = Drupal.slick.globals(self, merged);

        // Populate defaults + globals into breakpoints.
        if (typeof configs.responsive !== 'undefined') {
          $.map(configs.responsive, function (v, i) {
            if (typeof configs.responsive[i].settings !== 'undefined') {
              configs.responsive[i].settings = $.extend({}, defaults, configs.responsive[i].settings, globals);
            }
          });
        }

        Drupal.slick.randomize(t);

        // Build the Slick.
        t.slick($.extend(configs, globals, callbacks));

        // @todo drop if total <= slideToShow fixed, or onAfterChange works.
        $('.slick__slide', t).on('click', function () {
          $('.slide--current', t).removeClass('slide--current');
          $(this).addClass('slide--current');
        });

        // @todo drop when mousewheel does get in.
        // @see https://github.com/kenwheeler/slick/issues/122
        if ($.isFunction($.fn.mousewheel) && merged.mousewheel) {
          t.on('mousewheel', function(e, delta) {
            e.preventDefault();
            var wheelUp = (delta < 0) ? t.slickNext() : t.slickPrev();
          });
        }

        // Arrow down jumper.
        $('.jump-scroll[data-target]').click(function (e) {
          e.preventDefault();
          var a = $(this);
          $('html, body').stop().animate({
            scrollTop: $(a.data('target')).offset().top - (a.data('offset') || 0)
          }, 800, 'easeInOutExpo');
        });
      });
    }
  };

  /**
   * Randomize slide orders, useful to manipulate cached blocks.
   * @see https://github.com/kenwheeler/slick/issues/359
   * @see http://reddit.com/r/slickcarousel/comments/2kyu51/random_slide_order
   */
  Drupal.slick.randomize = function(t) {
    if (!t.hasClass('slick--random')) {
      return;
    }

    t.children('.slick__slide:not(.slick-cloned)').sort(function (){
      return Math.round(Math.random()) - 0.5;
    })
    .each(function (){
      $(this).appendTo(t);
    });
  };

  /**
   * Provides custom callbacks.
   * @see slick.api.js.
   * @see slick.media.js.
   */
  Drupal.slick.runCallbacks = function(slider, index) {
    if (Drupal.slick.callbacks) {
      var methods = $.each(Drupal.slick.callbacks, function(i, callback) {
        var name = {};
        if (typeof callback !== 'undefined' && typeof(callback) === 'function' && slider.$slider) {
         name[callback] = callback.call(this, index);
        }
      });
      return methods;
    }
  };

  /**
   * Gets slidesToShow depending on current settings.
   */
  Drupal.slick.toShow = function(merged) {
    var toShow = merged.slidesToShow;

    // Only rely on the first largest breakpoint, otherwise complex loop.
    if (typeof merged.responsive !== 'undefined' && typeof merged.responsive[0].breakpoint !== 'undefined') {
      if ($(window).width() <= merged.responsive[0].breakpoint) {
        toShow = merged.responsive[0].settings.slidesToShow;
      }
    }

    return parseInt(toShow);
  };

  /**
   * Update arrows.
   *   @see https://github.com/kenwheeler/slick/issues/745
   *   @todo drop if any fix.
   */
  Drupal.slick.updateArrows = function(t, merged, total) {
    var $arrows = $('.slick__arrow', t);
    if (!$arrows.length) {
      return;
    }

    var toShow = Drupal.slick.toShow(merged);

    // Do not remove arrows, to allow responsive option have different options.
    var arrows = total <= toShow ? $arrows.hide() : $arrows.show();
  };

  /**
   * Declare global options explicitly to copy into responsives.
   */
  Drupal.slick.globals = function(t, merged) {

    var globals = {
      asNavFor: merged.asNavFor,
      slide: merged.slide,
      lazyLoad: merged.lazyLoad,
      dotsClass: merged.dotsClass,
      rtl: merged.rtl,
      appendArrows: merged.appendArrows,
      prevArrow: $('.slick__arrow .slick-prev', t),
      nextArrow: $('.slick__arrow .slick-next', t),
      customPaging: function (slider, i) {
        return slider.$slides.eq(i).find('.slide__thumbnail--placeholder').html() || '<button type="button" data-role="none">' + (i + 1) + '</button>';
      },
      onInit: function (slider) {
        Drupal.theme('slickThumbnails', t);
        Drupal.slick.updateArrows(t, merged, slider.slideCount);
        // @todo drop if any fix for currentSlide active after initialized.
        // With centerMode.
        // When total = slidesToShow, the first + last have slick-center classes.
        // When total < slidesToShow, the first has slick-center class, even odd.
        $('.slick__slide[data-slick-index="' + slider.currentSlide + '"]', t).click().addClass('slide--current');
      },
      onReInit: function (slider) {
        Drupal.slick.updateArrows(t, merged, slider.slideCount);
      },
      onAfterChange: function (slider, index) {
        // @fixem, then @fixme.
        // If total < slidesToShow, onAfterChange uselessly triggered on load,
        // otherwise not working at all.
        Drupal.slick.setCurrent(t, slider.currentSlide);
      },
      onSetPosition: function (slider) {
        // @todo drop when onAfterChange works. Only if total > slidesToShow.
        Drupal.slick.setCurrent(t, slider.currentSlide);
      }
    };
    return globals;
  };

  /**
   * Without centerMode, .slick-active can be as many as visible slides, hence
   * added a specific class. Also fix for total <= slidesToShow with centerMode.
   */
  Drupal.slick.setCurrent = function(t, index) {
    $('.slide--current', t).removeClass('slide--current');
    $('.slick__slide[data-slick-index="' + index + '"]', t).addClass('slide--current');
  };

  /**
   * Theme function to update slick-dots to use thumbnail classes if available.
   */
  Drupal.theme.prototype.slickThumbnails = function (t) {
    // Do not proceed if no thumbnails available.
    if (!$('.slick__slide:first .slide__thumbnail', t).length) {
      return;
    }
    var dotClass = $(t).data('config').dotClass || 'slick-dots';
    $('.' + dotClass, t).addClass('slick__thumbnail');

    $('.slick__slide .slide__thumbnail--placeholder', t).remove();
  };

})(jQuery);