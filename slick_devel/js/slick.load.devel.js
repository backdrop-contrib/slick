/**
 * @file
 * Provides Slick loader.
 */

(function($, Drupal) {

  "use strict";

  Drupal.behaviors.slick = {
    attach: function(context, settings) {
      $('.slick', context).once('slick', function() {
        var _ = Drupal.slick,
          t = $('> .slick__slider', this),
          a = $('> .slick__arrow', this);

        // Build the Slick.
        _.beforeSlick(t, a);
        t.slick(_.globals(t, a));
        _.afterSlick(t);
      });
    }
  };

  Drupal.slick = {
    /**
     * The event must be bound prior to slick being called.
     */
    beforeSlick: function(t, a) {
      var _ = this,
        breakpoint;
      _.randomize(t);

      t.on('init', function(e, slick) {
        // Populate defaults + globals into each breakpoint.
        var sets = slick.options.responsive || null;
        if (sets && sets.length > -1) {
          for (breakpoint in sets) {
            if (sets.hasOwnProperty(breakpoint) && sets[breakpoint].settings !== 'unslick') {
              slick.breakpointSettings[sets[breakpoint].breakpoint] = $.extend({}, Drupal.settings.slick, _.globals(t, a), sets[breakpoint].settings);
            }
          }
        }

        // Update arrows with possible nested slick.
        if (t.attr('id') === slick.$slider.attr('id')) {
          _.arrows(a, slick);
        }
        _.setCurrent(t, slick.currentSlide);
      });

      t.on('beforeChange', function(e, slick, currentSlide, animSlide) {
        _.setCurrent(t, animSlide);
      });
    },

    /**
     * The event must be bound after slick being called.
     */
    afterSlick: function(t) {
      var _ = this,
        slick = t.slick('getSlick'),
        opt = _.options(slick);

      // Arrow down jumper.
      t.parent().on('click', '.jump-scroll[data-target]', function(e) {
        e.preventDefault();
        var b = $(this);
        $('html, body').stop().animate({
          scrollTop: $(b.data('target')).offset().top - (b.data('offset') || 0)
        }, 800, opt.easing || 'swing');
      });

      if ($.isFunction($.fn.mousewheel) && opt.mousewheel) {
        t.on('mousewheel', function(e, delta) {
          e.preventDefault();
          var wheeler = (delta < 0) ? t.slick('slickNext') : t.slick('slickPrev');
        });
      }

      t.trigger('afterSlick', [_, slick, slick.currentSlide]);
    },

    /**
     * Gets active options based on breakpoint, or fallback to global.
     */
    options: function(slick) {
      var breakpoint = slick.activeBreakpoint || null;
      return breakpoint && (slick.windowWidth <= breakpoint) ? slick.breakpointSettings[breakpoint] : slick.options;
    },

    /**
     * Randomize slide orders, for ads/products rotation within cached blocks.
     */
    randomize: function(t) {
      if (t.parent().hasClass('slick--random') && !t.hasClass('slick-initiliazed')) {
        t.children().sort(function() {
            return 0.5 - Math.random();
          })
          .each(function() {
            t.append(this);
          });
      }
    },

    /**
     * Fixed known arrows issue when total <= slidesToShow, and not updated.
     */
    arrows: function(a, slick) {
      var _ = this,
        opt = _.options(slick);
      a.find('>*:not(.slick-down)').addClass('slick-nav');
      // Do not remove arrows, to allow responsive have different options.
      var arrows = slick.slideCount <= opt.slidesToShow || opt.arrows === false ? a.hide() : a.show();
    },

    /**
     * Sets the current slide class.
     *
     * Without centerMode, .slick-active can be as many as visible slides, hence
     * added a specific class. Also fix total <= slidesToShow with centerMode.
     * slick-current class is finally added 5/24/15.
     *
     * @todo deprecate slide--current for slick-current from v1.5.6.
     *
     * @see https://github.com/kenwheeler/slick/issues/1248
     * @see https://github.com/kenwheeler/slick/commit/7eadb8adac811f573d69626ceac8efda047a7bb9
     */
    setCurrent: function(t, curr) {
      // Must take care for both asNavFor instances, with/without slick-wrapper.
      var w = t.parent('.slick').parent();
      $('.slick-slide', w).removeClass('slide--current');
      $('[data-slick-index="' + curr + '"]', w).addClass('slide--current');
    },

    /**
     * Declare global options explicitly to copy into responsive settings.
     */
    globals: function(t, a) {
      var merged = $.extend({}, Drupal.settings.slick, t.data('slick'));
      return {
        slide: merged.slide,
        lazyLoad: merged.lazyLoad,
        dotsClass: merged.dotsClass,
        rtl: merged.rtl,
        appendDots: merged.appendDots || $(t),
        prevArrow: $('.slick-prev', a),
        nextArrow: $('.slick-next', a),
        appendArrows: a,
        customPaging: function(slick, i) {
          var tn = slick.$slides.eq(i).find('[data-thumb]') || null,
            dots_thumb = tn.length && merged.dotsClass.indexOf('thumbnail') > 0 ?
              '<div class="slick-dots__thumbnail"><img alt="' + Drupal.t(tn.attr('alt')) + '" src="' + tn.data('thumb') + '"></div>' : '';
          return dots_thumb + slick.defaults.customPaging(slick, i);
        }
      };
    }
  };

})(jQuery, Drupal);
