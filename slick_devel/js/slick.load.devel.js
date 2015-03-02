/**
 * @file
 * Provides Slick loader.
 */

(function ($, Drupal, window) {

  "use strict";

  Drupal.slick = Drupal.slick || {};

  Drupal.behaviors.slick = {
    attach: function(context, settings) {

      $('.slick', context).once('slick', function() {
        var t = $('> .slick__slider', this),
          a = $('~ .slick__arrow', t),
          configs = t.data('config') || {},
          merged = $.extend({}, settings.slick, configs),
          globals = Drupal.slick.globals(t, a, merged);

        // Populate defaults + globals into each breakpoint.
        if (typeof configs.responsive !== 'undefined') {
          $.map(configs.responsive, function(v, i) {
            if (typeof configs.responsive !== 'undefined' && typeof configs.responsive[i].settings !== 'undefined' && configs.responsive[i].settings !== 'unslick') {
              configs.responsive[i].settings = $.extend({}, settings.slick, globals, configs.responsive[i].settings);
            }
          });
        }

        // Build the Slick.
        Drupal.slick.beforeSlick(t, a);
        t.slick($.extend(globals, configs));
        Drupal.slick.afterSlick(t);
      });
    }
  };

  Drupal.slick = {

    /**
     * The event must be bound prior to slick being called.
     */
    beforeSlick: function(t, a) {
      Drupal.slick.randomize(t);

      t.on('init', function(e, slick) {
        var options = Drupal.slick.options(slick);
        Drupal.slick.thumbnail(t, options.dotsClass);
        Drupal.slick.arrows(a, slick.slideCount, options);
      });
    },

    /**
     * The event must be bound after slick being called.
     */
    afterSlick: function(t) {
      var slider = t.slick('getSlick'),
        options = Drupal.slick.options(slider);

      Drupal.slick.setCurrent(t, options.initialSlide);

      t.on('afterChange', function(e, slick, currentSlide) {
        Drupal.slick.setCurrent(t, currentSlide);
      });

      if (options.focusOnSelect && (slider.slideCount <= options.slidesToShow)) {
        t.on('click', '.slick-slide', function(e) {
          Drupal.slick.setCurrent(t, $(this).data('slickIndex'));
        });
      }

      // Arrow down jumper.
      t.parent().on('click', '.jump-scroll[data-target]', function(e) {
        e.preventDefault();
        var a = $(this);
        $('html, body').stop().animate({
          scrollTop: $(a.data('target')).offset().top - (a.data('offset') || 0)
        }, 800, options.easing || 'swing');
      });

      if ($.isFunction($.fn.mousewheel) && options.mousewheel == true) {
        t.on('mousewheel', function(e, delta) {
          e.preventDefault();
          var wheeler = (delta < 0) ? t.slick('slickNext') : t.slick('slickPrev');
        });
      }
    },

    /**
     * Gets active options based on breakpoint, or fallback to global.
     */
    options: function(slider) {
      var breakpoint = slider.activeBreakpoint || null;
      return breakpoint && slider.windowWidth < breakpoint ? slider.breakpointSettings[breakpoint] : slider.options;
    },

    /**
     * Randomize slide orders, for ads/products rotation within cached blocks.
     */
    randomize: function(t) {
      if (!t.parent().hasClass('slick--random')) {
        return;
      }

      t.children('> .slick__slide:not(.slick-cloned)').sort(function() {
          return Math.round(Math.random()) - 0.5;
        })
        .each(function() {
          $(this).appendTo(t);
        });
    },

    /**
     * Fixed core bug with arrows when total <= slidesToShow, and not updated.
     */
    arrows: function(a, total, options) {
      a.find('>*').addClass('slick-nav');
      // Do not remove arrows, to allow responsive have different options.
      var arrows = total <= options.slidesToShow || options.arrows === false ? a.hide() : a.show();
    },

    /**
     * Update slick-dots to use thumbnail classes if available.
     */
    thumbnail: function(t, dotsClass) {
      if ($('.slick__slide:first .slide__thumbnail', t).length) {
        $('> .' + dotsClass, t).addClass('slick__thumbnail');
        $('.slick__slide .slide__thumbnail--placeholder', t).hide();
      }
    },

    /**
     * Returns the current slide class.
     *
     * Without centerMode, .slick-active can be as many as visible slides.
     * added a specific class. Also fix total <= slidesToShow with centerMode.
     */
    setCurrent: function(t, curr) {
      // Must take care for both asNavFor instances, with/without slick-wrapper.
      var w = t.parent('.slick').parent();
      $('.slick__slide', w).removeClass('slide--after slide--before slide--current');
      var $curr = $('[data-slick-index="' + curr + '"]', w).addClass('slide--current');
      $curr.prevAll().addClass('slide--before');
      $curr.nextAll().addClass('slide--after');
    },

    /**
     * Declare global options explicitly to copy into responsives.
     */
    globals: function(t, a, merged) {
      var globals = {
        slide: merged.slide,
        lazyLoad: merged.lazyLoad,
        dotsClass: merged.dotsClass,
        rtl: merged.rtl,
        appendDots: merged.appendDots || $(t),
        prevArrow: $('.slick-prev', a),
        nextArrow: $('.slick-next', a),
        appendArrows: a,
        customPaging: function(slick, i) {
          return slick.$slides.eq(i).find('.slide__thumbnail--placeholder').html() || slick.defaults.customPaging(slick, i);
        }
      };
      return globals;
    }
  };

})(jQuery, Drupal, this);
