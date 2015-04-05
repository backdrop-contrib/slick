/**
 * @file
 * Provides Slick loader.
 */

(function($, Drupal) {

  "use strict";

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
      var _ = this;
      _.randomize(t);

      t.on('init', function(e, slick) {
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
        var a = $(this);
        $('html, body').stop().animate({
          scrollTop: $(a.data('target')).offset().top - (a.data('offset') || 0)
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
      return breakpoint && (slick.windowWidth < breakpoint) ? slick.breakpointSettings[breakpoint] : slick.options;
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
     * Returns the current slide class.
     *
     * Without centerMode, .slick-active can be as many as visible slides, hence
     * added a specific class. Also fix total <= slidesToShow with centerMode.
     * Given different situations, as of v1.5 Master, still can't interact with
     * internal selectors consistently, e.g.: slick.$slides.eq(curr).
     */
    setCurrent: function(t, curr) {
      // Must take care for both asNavFor instances, with/without slick-wrapper.
      var w = t.parent('.slick').parent();
      $('.slick-slide', w).removeClass('slide--current');
      $('[data-slick-index="' + curr + '"]', w).addClass('slide--current');
    },

    /**
     * Declare global options explicitly to copy into responsives.
     */
    globals: function(t, a, merged) {
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
