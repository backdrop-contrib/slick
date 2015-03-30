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

      var $form = $('.form--slick').removeClass('no-js');
      $('.fieldset-legend-prefix', $form).removeClass('element-invisible');
      $('.form-checkboxes .form-checkbox', $form).removeClass('is-tooltip');

      $('.form-checkbox', $form).once('slick-checkbox', function () {
        var $checkbox = $(this);
        if (!$checkbox.next('.field-suffix').length) {
          $checkbox.after('<span class="field-suffix"></span>');
        }

        $checkbox.click(function () {
          var t = $(this);
          if (t.prop('checked')) {
            t.addClass('on');
          }
          else {
            t.removeClass('on');
          }
        });
      });

      $('.is-tooltip', $form).once('slick-tooltip', function () {
        var t = $(this);
        if (!t.closest('.form-item').find('> .hint').length) {
          t.closest('.form-item').append('<span class="hint">?</span>');
        }

        $('~ .hint', t).hover(function () {
          $(this).closest('.form-item').addClass('hover');
        },
        function () {
          $(this).closest('.form-item').removeClass('hover');
        });

        $('~ .hint', t).click(function () {
          $('.form--slick .form-item.selected').removeClass('selected');
          $(this).parent().toggleClass('selected');
        });

        $('~ .description', t).click(function () {
          $(this).parent().removeClass('selected');
        });
      });

      $('.form--slick', context).once('slick-admin', function () {
        var t = $(this);

        $('.form-type-textfield > .form-text.js-expandable', t).focus(function () {
          $(this).parent().addClass('js-on-focus');
        }).blur(function () {
          $(this).parent().removeClass('js-on-focus');
        });
      });
    }
  };

})(jQuery);
