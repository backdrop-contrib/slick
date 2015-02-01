/**
 * @file
 */
(function ($, Drupal, window) {
  "use strict";

  Drupal.slick = Drupal.slick || {};

  Drupal.behaviors.slick = {
    attach: function(context, settings) {

      $('.slick', context).once('slick', function() {
        var t = $('.slick__slider', this),
          configs = t.data('config') || {},
          merged = $.extend({}, settings.slick, configs),
          globals = Drupal.slick.globals(this, merged);

        // Populate defaults + globals into breakpoints.
        if (typeof configs.responsive !== 'undefined') {
          $.map(configs.responsive, function(v, i) {
            if (typeof configs.responsive[i].settings !== 'undefined' && configs.responsive[i].settings !== 'unslick') {
              configs.responsive[i].settings = $.extend({}, settings.slick, configs.responsive[i].settings, globals);
            }
          });
        }

        // Build the Slick.
        Drupal.slick.beforeSlick(t, merged);
        t.slick($.extend(configs, globals));
        Drupal.slick.afterSlick(t, merged);
      });
    }
  };

  Drupal.slick = {

    /**
     * The event must be bound prior to slick being called.
     */
    beforeSlick: function(t, merged) {
      Drupal.slick.randomize(t);

      t.on('init', function(e, slick) {
        Drupal.slick.thumbnail(t, merged);
        Drupal.slick.arrows(t, merged, slick.slideCount);
      });
    },

    /**
     * The event must be bound after slick being called.
     */
    afterSlick: function(t, merged) {
      $('.slide--' + merged.initialSlide, t).addClass('slide--current');

      t
        .on('reInit', function(e, slick) {
          Drupal.slick.arrows(t, merged, slick.slideCount);
        })
        .on('beforeChange', function(e, slick, currentSlide, nextSlide) {
          $('.slide--current', t).removeClass('slide--current');
        })
        .on('afterChange', function(e, slick, currentSlide) {
          Drupal.slick.setCurrent(t, currentSlide);
        })
        .on('click.slick-slide', '.slick__slide', function(e) {
          Drupal.slick.setCurrent(t, parseInt($(this).data('slickIndex')));
        });

      // Arrow down jumper.
      t.parent().on('click', '.jump-scroll[data-target]', function(e) {
        e.preventDefault();
        var a = $(this);
        $('html, body').stop().animate({
          scrollTop: $(a.data('target')).offset().top - (a.data('offset') || 0)
        }, 800, 'easeInOutExpo');
      });

      if ($.isFunction($.fn.mousewheel) && merged.mousewheel) {
        var slider = t.slick('getSlick');
        t.on('mousewheel', function(e, delta) {
          e.preventDefault();
          var wheeler = (delta < 0) ? slider.slickNext() : slider.slickPrev();
        });
      }
    },

    /**
     * Randomize slide orders, useful to manipulate cached blocks with ondemand.
     */
    randomize: function(t) {
      if (!t.parent().hasClass('slick--random')) {
        return;
      }

      t.children('.slick__slide:not(.slick-cloned)').sort(function() {
          return Math.round(Math.random()) - 0.5;
        })
        .each(function() {
          $(this).appendTo(t);
        });
    },

    /**
     * Update arrows.
     */
    arrows: function(t, merged, total) {
      var $arrows = $('.slick__arrow', t);
      if (!$arrows.length) {
        return;
      }

      // Gets slidesToShow depending on current settings.
      var toShow = merged.slidesToShow;
      if (typeof merged.responsive !== 'undefined' && typeof merged.responsive[0].breakpoint !== 'undefined') {
        if ($(window).width() <= merged.responsive[0].breakpoint) {
          toShow = merged.responsive[0].settings.slidesToShow;
        }
      }
      toShow = parseInt(toShow);

      // Do not remove arrows, to allow responsive have different options.
      var arrows = total <= toShow ? $arrows.hide() : $arrows.show();
    },

    /**
     * Update slick-dots to use thumbnail classes if available.
     */
    thumbnail: function(t, merged) {
      if ($('.slick__slide:first .slide__thumbnail', t).length) {
        $('.' + merged.dotsClass, t).addClass('slick__thumbnail');
        $('.slick__slide .slide__thumbnail--placeholder', t).remove();
      }
    },

    /**
     * Without centerMode, .slick-active can be as many as visible slides, hence
     * added a specific class. Also fix for total <= slidesToShow with centerMode.
     */
    setCurrent: function(t, curr) {
      $('.slide--current', t).removeClass('slide--current');
      $('.slide--' + curr, t).addClass('slide--current');
    },

    /**
     * Declare global options explicitly to copy into responsives.
     */
    globals: function(t, merged) {
      var globals = {
        asNavFor: merged.asNavFor,
        slide: merged.slide,
        lazyLoad: merged.lazyLoad,
        dotsClass: merged.dotsClass,
        rtl: merged.rtl,
        prevArrow: $('.slick__arrow .slick-prev', t) || merged.prevArrow,
        nextArrow: $('.slick__arrow .slick-next', t) || merged.nextArrow,
        appendArrows: merged.appendArrows,
        customPaging: function(slick, i) {
          return slick.$slides.eq(i).find('.slide__thumbnail--placeholder').html() || '<button type="button" data-role="none">' + (i + 1) + '</button>';
        }
      };

      return globals;
    }
  };

})(jQuery, Drupal, this);