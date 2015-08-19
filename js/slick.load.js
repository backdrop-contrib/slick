/**
 * @file
 * Provides Slick loader.
 */

/*jshint -W072 */
/*eslint max-params: 0 */
(function ($, Drupal) {

  "use strict";

  Drupal.behaviors.slick = {
    attach: function (context) {
      var _ = this;
      $(".slick:not(.unslick)", context).once("slick", function () {
        var $this = $(this),
          t = $("> .slick__slider", $this),
          a = $("> .slick__arrow", $this);

        // Build the Slick.
        _.beforeSlick(t, a);
        t.slick(_.globals(t, a));
        _.afterSlick(t);
      });
    },

    /**
     * The event must be bound prior to slick being called.
     */
    beforeSlick: function (t, a) {
      var _ = this,
        breakpoint;
      _.randomize(t);

      t.on("init", function (e, slick) {
        // Populate defaults + globals into each breakpoint.
        var sets = slick.options.responsive || null;
        if (sets && sets.length > -1) {
          for (breakpoint in sets) {
            if (sets.hasOwnProperty(breakpoint)
              && sets[breakpoint].settings !== "unslick") {
              slick.breakpointSettings[sets[breakpoint].breakpoint] = $.extend(
                {},
                Drupal.settings.slick,
                _.globals(t, a),
                sets[breakpoint].settings);
            }
          }
        }

        _.setCurrent(t, slick.currentSlide, slick);
      });

      t.on("beforeChange", function (e, slick, currentSlide, animSlide) {
        _.setCurrent(t, animSlide, slick);
      });

      // Fixed known arrows issue when total <= slidesToShow, and not updated.
      t.on("setPosition", function (e, slick) {
        var opt = _.options(slick);

        // Do not remove arrows, to allow responsive have different options.
        if (t.attr("id") === slick.$slider.attr("id")) {
          return slick.slideCount <= opt.slidesToShow || opt.arrows === false
            ? a.addClass("element-hidden") : a.removeClass("element-hidden");
        }
      });
    },

    /**
     * The event must be bound after slick being called.
     */
    afterSlick: function (t) {
      var _ = this,
        slick = t.slick("getSlick"),
        opt = _.options(slick);

      // Arrow down jumper.
      t.parent().on("click.slick-load", ".jump-scroll[data-target]", function (e) {
        e.preventDefault();
        var b = $(this);
        $("html, body").stop().animate({
          scrollTop: $(b.data("target")).offset().top - (b.data("offset") || 0)
        }, 800, opt.easing || "swing");
      });

      if ($.isFunction($.fn.mousewheel) && opt.mousewheel) {
        t.on("mousewheel.slick-load", function (e, delta) {
          e.preventDefault();
          return (delta < 0) ? t.slick("slickNext") : t.slick("slickPrev");
        });
      }

      t.trigger("afterSlick", [_, slick, slick.currentSlide]);
    },

    /**
     * Gets active options based on breakpoint, or fallback to global.
     */
    options: function (slick) {
      var breakpoint = slick.activeBreakpoint || null;
      return breakpoint && (slick.windowWidth <= breakpoint)
        ? $.extend({},
        Drupal.settings.slick, slick.breakpointSettings[breakpoint])
        : slick.options;
    },

    /**
     * Randomize slide orders, for ads/products rotation within cached blocks.
     */
    randomize: function (t) {
      if (t.parent().hasClass("slick--random")
        && !t.hasClass("slick-initiliazed")) {
        t.children().sort(function () {
            return 0.5 - Math.random();
          })
          .each(function () {
            t.append(this);
          });
      }
    },

    /**
     * Sets the current slide class.
     *
     * Still kept after v1.5.8 (8/4) as "slick-current" fails with asNavFor:
     *   - Create asNavFor with the total <= slidesToShow and centerMode.
     *   - Drag the main large display, or click its arrows, thumbnail
     *     slick-current class is not updated/ synched, always stucked at 0.
     * Or else, drop slide--current, and just push slick-current where it fails.
     *
     * @todo deprecate slide--current for slick-current from v1.5.8+ if fixed.
     */
    setCurrent: function (t, curr, slick) {
      // Must take care for both asNavFor instances, with/without slick-wrapper.
      var w = t.parent(".slick").parent();
      // Be sure the most complex slicks are taken care of as well, e.g.:
      // asNavFor with the main display containing nested slicks.
      if (t.attr("id") === slick.$slider.attr("id")) {
        $(".slick-slide", w).removeClass("slide--current");
        $("[data-slick-index='" + curr + "']", w).addClass("slide--current");
      }
    },

    /**
     * Declare global options explicitly to copy into responsive settings.
     */
    globals: function (t, a) {
      var merged = $.extend({}, Drupal.settings.slick, t.data("slick"));
      return {
        slide: merged.slide,
        lazyLoad: merged.lazyLoad,
        dotsClass: merged.dotsClass,
        rtl: merged.rtl,
        appendDots: merged.appendDots === ".slick__arrow"
          ? a : (merged.appendDots || $(t)),
        prevArrow: $(".slick-prev", a),
        nextArrow: $(".slick-next", a),
        appendArrows: a,
        customPaging: function (slick, i) {
          var tn = slick.$slides.eq(i).find("[data-thumb]") || null,
            alt = Drupal.t(tn.attr("alt")) || "",
            img = "<img alt='" + alt + "' src='" + tn.data("thumb") + "'>",
            dotsThumb = tn.length && merged.dotsClass.indexOf("thumbnail") > 0 ?
              "<div class='slick-dots__thumbnail'>" + img + "</div>" : "";
          return dotsThumb + slick.defaults.customPaging(slick, i);
        }
      };
    }
  };

})(jQuery, Drupal);
