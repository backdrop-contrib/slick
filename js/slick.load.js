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

        // Populate defaults + globals into each breakpoint.
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
      var slider = t.slick('getSlick');
      Drupal.slick.setCurrent(t, merged.initialSlide);

      t.on('afterChange', function(e, slick, currentSlide) {
        Drupal.slick.setCurrent(t, currentSlide);
      });

      if (merged.focusOnSelect && (slider.slideCount <= Drupal.slick.toShow(t, merged))) {
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
        }, 800, merged.easing || 'swing');
      });

      if ($.isFunction($.fn.mousewheel) && merged.mousewheel) {
        t.on('mousewheel', function(e, delta) {
          e.preventDefault();
          var wheeler = (delta < 0) ? t.slick('slickNext') : t.slick('slickPrev');
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
     * Gets slidesToShow depending on current settings.
     */
    toShow: function(t, merged) {
      var toShow = merged.slidesToShow;
      if (typeof merged.responsive !== 'undefined' && typeof merged.responsive[0].breakpoint !== 'undefined') {
        if ($(window).width() <= merged.responsive[0].breakpoint) {
          toShow = merged.responsive[0].settings.slidesToShow;
        }
      }
      return parseInt(toShow);
    },

    /**
     * Fixed core bug with arrows when total <= slidesToShow.
     */
    arrows: function(t, merged, total) {
      var $arrows = $('.slick__arrow', t.parent());
      if (!$arrows.length) {
        return;
      }

      // Do not remove arrows, to allow responsive have different options.
      var arrows = total <= Drupal.slick.toShow(t, merged) ? $arrows.hide() : $arrows.show();
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
     * added a specific class. Also fix total <= slidesToShow with centerMode.
     */
    setCurrent: function(t, curr) {
      // Must take care for both asNavFor as well.
      var w = t.closest('.slick-wrapper');
      $('.slick__slide', w).removeClass('slide--after slide--before slide--current');
      var $curr = $('[data-slick-index="' + curr + '"]', w).addClass('slide--current');
      $curr.prevAll().addClass('slide--before');
      $curr.nextAll().addClass('slide--after');
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
        prevArrow: $('.slick__arrow .slick-prev', t),
        nextArrow: $('.slick__arrow .slick-next', t),
        appendArrows: merged.appendArrows,
        customPaging: function(slick, i) {
          return slick.$slides.eq(i).find('.slide__thumbnail--placeholder').html() || slick.defaults.customPaging(slick, i);
        }
      };

      return globals;
    }
  };

})(jQuery, Drupal, this);