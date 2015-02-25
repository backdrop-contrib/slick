/**
 * @file
 * Provides admin untilities.
 */

(function($) {
  "use strict";

  // Supports for jQuery < 1.6 admin pages.
  if (typeof $.fn.prop !== 'function') {
    $.fn.prop = function(name, value){
      if (typeof value === 'undefined') {
        return this.attr(name);
      }
      else {
        return this.attr(name, value);
      }
    };
  }

  Drupal.behaviors.slickAdmin = {
    attach: function(context, settings) {

      if (!$('html').hasClass('js')) {
        $('html').addClass('js');
      }

      $('.form--slick', context).removeClass('no-js').once('slick-admin', function () {
        var t = $(this);
        $('.fieldset-legend-prefix', t).removeClass('element-invisible');

        $('.is-tooltip', t).each(function () {
          if (!$(this).closest('.form-item').find('.hint').length) {
            $(this).closest('.form-item:not(.form-checkboxes .form-type-checkbox)').append('<span class="hint">?</span>');
          }
        });

        $('.form-item > .form-checkbox', t).click(function () {
          var t = $(this);
          if (t.prop('checked')) {
            t.addClass('on');
          }
          else {
            t.removeClass('on');
          }
        });

        $('.form-item > .hint', t).hover(function () {
          $(this).closest('.form-item').addClass('hover');
        },
        function () {
          $(this).closest('.form-item').removeClass('hover');
        });

        $('.form-item > .hint', t).click(function () {
          $('.form--slick .form-item.selected').removeClass('selected');
          $(this).parent().toggleClass('selected');
        });

        $('.form-item > .description', t).click(function () {
          $(this).parent().removeClass('selected');
        });

        $('.form-type-textfield > .form-text.js-expandable', t).focus(function () {
          $(this).parent().addClass('js-on-focus');
        }).blur(function () {
          $(this).parent().removeClass('js-on-focus');
        });
      });
    }
  };

})(jQuery);
