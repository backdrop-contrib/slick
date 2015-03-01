<?php

/**
 * @file
 * Contains the CTools Export UI integration code.
 */

/**
 * CTools Export UI class handler for Slick Optionset UI.
 */
class SlickOptionsetUi extends ctools_export_ui {

  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    ctools_form_include($form_state, 'slick.admin', 'slick');

    $module_path = drupal_get_path('module', 'slick');
    $optionset = $form_state['item'];

    $options = $optionset->options;
    $form['#attached']['library'][] = array('slick', 'slick.admin');
    $form['#attached']['css'][] = $module_path . '/css/admin/slick.admin--vertical-tabs.css';

    $form['#attributes']['class'][] = 'no-js';
    $form['#attributes']['class'][] = 'form--slick';
    $form['#attributes']['class'][] = 'form--compact';
    $form['#attributes']['class'][] = 'form--optionset';
    $form['#attributes']['class'][] = 'clearfix';

    $form['info']['name']['#attributes']['class'][] = 'is-tooltip';
    $form['info']['label']['#attributes']['class'][] = 'is-tooltip';
    $form['info']['label']['#prefix'] = '<div class="form--slick__header has-tooltip clearfix">';

    // Skins. We don't provide skin_thumbnail as each optionset may be deployed
    // as main or thumbnail navigation.
    $skins = slick_skins(TRUE);
    $form['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin'),
      '#options' => $skins,
      '#default_value' => $optionset->skin,
      '#empty_option' => t('- None -'),
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. Make sure to provide a dedicated slide layout per field. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself. See main <a href="@skin">README</a> for details on Skins. Keep it simple for thumbnail navigation skin.', array('@skin' => url($module_path . '/README.txt'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['breakpoints'] = array(
      '#title' => t('Breakpoints'),
      '#type' => 'textfield',
      '#description' => t('The number of breakpoints added to Responsive display, max 9. This is not Breakpoint Width (480px, etc).'),
      '#default_value' => isset($form_state['values']['breakpoints']) ? $form_state['values']['breakpoints'] : $optionset->breakpoints,
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'slick_add_breakpoints_ajax_callback',
        'wrapper' => 'breakpoints-ajax-wrapper',
        'event' => 'blur',
      ),
      '#attributes' => array('class' => array('is-tooltip')),
      '#maxlength' => 1,
    );

    // Options.
    $form['options'] = array(
      '#type' => 'vertical_tabs',
      '#tree' => TRUE,
    );

    // Image styles.
    $image_styles = function_exists('image_style_options') ? image_style_options(FALSE) : array();
    $form['options']['general'] = array(
      '#type' => 'fieldset',
      '#title' => t('General'),
      '#attributes' => array('class' => array('has-tooltip', 'fieldset--no-checkboxes-label')),
    );

    $form['options']['general']['normal'] = array(
      '#type' => 'select',
      '#title' => t('Image style'),
      '#description' => t('Image style for the main/background image, overriden by field formatter.'),
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
      '#default_value' => isset($options['general']['normal']) ? $options['general']['normal'] : '',
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // More useful for custom work, overriden by sub-modules.
    $form['options']['general']['thumbnail'] = array(
      '#type' => 'select',
      '#title' => t('Thumbnail style'),
      '#description' => t('Image style for the thumbnail image if using asNavFor, overriden by field formatter.'),
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
      '#default_value' => isset($options['general']['thumbnail']) ? $options['general']['thumbnail'] : '',
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['options']['general']['template_class'] = array(
      '#type' => 'textfield',
      '#title' => t('Wrapper class'),
      '#description' => t('Additional template wrapper classes separated by spaces. No need to prefix it with a dot (.). Use it in conjunction with asNavFor as needed, e.g.: <em>slick--for</em> for the main display, and <em>slick--nav</em> for thumbnail navigation.'),
      '#default_value' => isset($options['general']['template_class']) ? $options['general']['template_class'] : '',
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['options']['general']['goodies'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Goodies'),
      '#default_value' => !empty($options['general']['goodies']) ? array_values((array) $options['general']['goodies']) : array(),
      '#options' => array(
        'pattern' => t('Use pattern overlay'),
        'arrow-down' => t('Use arrow down'),
        'random' => t('Randomize'),
      ),
      '#description' => t('Applies to main display, not thumbnail pager. <ol><li>Pattern overlay is background image with pattern placed over the main stage.</li><li>Arrow down to scroll down into a certain page section, make sure to provide target selector.</li><li>Randomize the slide display, useful to manipulate cached blocks.</li></ol>'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['options']['general']['arrow_down_target'] = array(
      '#type' => 'textfield',
      '#title' => t('Arrow down target'),
      '#description' => t('Valid CSS selector to scroll to, e.g.: #main, or #content.'),
      '#default_value' => isset($options['general']['arrow_down_target']) ? $options['general']['arrow_down_target'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name*=arrow-down]' => array('checked' => TRUE),
        ),
      ),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['options']['general']['arrow_down_offset'] = array(
      '#type' => 'textfield',
      '#title' => t('Arrow down offset'),
      '#description' => t('Offset when scrolled down from the top.'),
      '#default_value' => isset($options['general']['arrow_down_offset']) ? $options['general']['arrow_down_offset'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name*=arrow-down]' => array('checked' => TRUE),
        ),
      ),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($form['options']['general']['goodies']['#options'] as $key => $value) {
      $form['options']['general']['goodies'][$key]['#field_suffix'] = '';
      $form['options']['general']['goodies'][$key]['#title_display'] = 'before';
    }

    // Main options.
    $slick_elements = $this->getSlickElements();
    $form['options']['settings'] = array(
      '#title' => t('Settings'),
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#attributes' => array('class' => array('fieldset--settings', 'has-tooltip')),
    );

    foreach ($slick_elements as $name => $element) {
      $default_value = isset($options['settings'][$name]) ? $options['settings'][$name] : $element['default'];
      $form['options']['settings'][$name] = array(
        '#title' => isset($element['title']) ? $element['title'] : '',
        '#description' => isset($element['description']) ? $element['description'] : '',
        '#type' => $element['type'],
        '#default_value' => $default_value,
        '#attributes' => array('class' => array('is-tooltip')),
      );

      if (isset($element['field_suffix'])) {
        $form['options']['settings'][$name]['#field_suffix'] = $element['field_suffix'];
      }

      if ($element['type'] == 'textfield') {
        $form['options']['settings'][$name]['#size'] = 20;
        $form['options']['settings'][$name]['#maxlength'] = 255;
      }

      if (!isset($element['field_suffix']) && $element['cast'] == 'bool') {
        $form['options']['settings'][$name]['#field_suffix'] = '';
        $form['options']['settings'][$name]['#title_display'] = 'before';
      }

      if ($element['cast'] == 'int') {
        $form['options']['settings'][$name]['#maxlength'] = 60;
        $form['options']['settings'][$name]['#attributes']['class'][] = 'form-text--int';
      }

      if (isset($element['states'])) {
        $form['options']['settings'][$name]['#states'] = $element['states'];
      }

      if (isset($element['options'])) {
        $form['options']['settings'][$name]['#options'] = $element['options'];
      }

      if (isset($element['empty_option'])) {
        $form['options']['settings'][$name]['#empty_option'] = $element['empty_option'];
      }

      // Expand textfield for easy edit.
      if (in_array($name, array('prevArrow', 'nextArrow'))) {
        $form['options']['settings'][$name]['#attributes']['class'][] = 'js-expandable';
      }
    }

    // Responsive options.
    $form['options']['responsives'] = array(
      '#title' => t('Responsive display'),
      '#type' => 'fieldset',
      '#description' => t('Containing breakpoints and settings objects. Settings set at a given breakpoint/screen width is self-contained and does not inherit the main settings, but defaults.'),
      '#collapsible' => FALSE,
      '#tree' => TRUE,
    );

    $form['options']['responsives']['responsive'] = array(
      '#title' => t('Responsive'),
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#attributes' => array('class' => array('has-tooltip', 'fieldset--responsive--ajax')),
      '#prefix' => '<div id="breakpoints-ajax-wrapper">',
      '#suffix' => '</div>',
    );

    $breakpoints_count = isset($form_state['values']['breakpoints']) ? $form_state['values']['breakpoints'] : $optionset->breakpoints;
    $form_state['breakpoints_count'] = $breakpoints_count;

    if ($form_state['breakpoints_count'] > 0) {
      $slick_responsive_elements = $this->getSlickResponsiveElements($form_state['breakpoints_count']);

      foreach ($slick_responsive_elements as $i => $responsives) {
        // Invidual breakpoint fieldset.
        $fieldset_class = drupal_clean_css_identifier(drupal_strtolower($responsives['title']));
        $form['options']['responsives']['responsive'][$i] = array(
          '#title' => $responsives['title'],
          '#type' => $responsives['type'],
          '#description' => isset($responsives['description']) ? $responsives['description'] : '',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#attributes' => array('class' => array('fieldset--responsive', 'fieldset--' . $fieldset_class, 'has-tooltip')),
        );

        foreach ($responsives as $key => $responsive) {
          switch ($key) {
            case 'breakpoint':
            case 'unslick':
              $form['options']['responsives']['responsive'][$i][$key] = array(
                '#title' => $responsive['title'],
                '#description' => $responsive['description'],
                '#type' => $responsive['type'],
                '#default_value' => isset($options['responsives']['responsive'][$i][$key]) ? $options['responsives']['responsive'][$i][$key] : $responsive['default'],
                '#attributes' => array('class' => array('is-tooltip')),
              );

              if ($responsive['type'] == 'textfield') {
                $form['options']['responsives']['responsive'][$i][$key]['#size'] = 20;
                $form['options']['responsives']['responsive'][$i][$key]['#maxlength'] = 255;
              }
              if ($responsive['cast'] == 'int') {
                $form['options']['responsives']['responsive'][$i][$key]['#maxlength'] = 60;
              }
              if (isset($responsive['states'])) {
                $form['options']['responsives']['responsive'][$i][$key]['#states'] = $responsive['states'];
              }
              if (isset($responsive['options'])) {
                $form['options']['responsives']['responsive'][$i][$key]['#options'] = $responsive['options'];
              }
              if (isset($responsive['field_suffix'])) {
                $form['options']['responsives']['responsive'][$i][$key]['#field_suffix'] = $responsive['field_suffix'];
              }
              if (!isset($responsive['field_suffix']) && $responsive['cast'] == 'bool') {
                $form['options']['responsives']['responsive'][$i][$key]['#field_suffix'] = '';
                $form['options']['responsives']['responsive'][$i][$key]['#title_display'] = 'before';
              }
              break;

            case 'settings':
              $form['options']['responsives']['responsive'][$i][$key] = array(
                '#title' => t('Settings'),
                '#title_display' => 'invisible',
                '#type' => 'fieldset',
                '#collapsible' => FALSE,
                '#collapsed' => FALSE,
                '#attributes' => array('class' => array('fieldset--settings', 'fieldset--' . $fieldset_class, 'has-tooltip')),
                '#states' => array('visible' => array(':input[name*="[responsive][' . $i . '][unslick]"]' => array('checked' => FALSE))),
              );
              unset($responsive['title'], $responsive['type']);

              if (!is_array($responsive)) {
                continue;
              }
              foreach ($responsive as $k => $item) {
                if ($item && !is_array($item)) {
                  continue;
                }
                $form['options']['responsives']['responsive'][$i][$key][$k] = array(
                  '#title' => isset($item['title']) ? $item['title'] : '',
                  '#description' => isset($item['description']) ? $item['description'] : '',
                  '#type' => $item['type'],
                  '#attributes' => array('class' => array('is-tooltip')),
                  '#default_value' => isset($options['responsives']['responsive'][$i][$key][$k]) ? $options['responsives']['responsive'][$i][$key][$k] : $item['default'],
                );

                // Specify proper states for the breakpoint elements.
                if (isset($item['states'])) {
                  $states = '';
                  switch ($k) {
                    case 'pauseOnHover':
                    case 'pauseOnDotsHover':
                    case 'autoplaySpeed':
                      $states = array('visible' => array(':input[name*="[' . $i . '][settings][autoplay]"]' => array('checked' => TRUE)));
                      break;

                    case 'centerPadding':
                      $states = array('visible' => array(':input[name*="[' . $i . '][settings][centerMode]"]' => array('checked' => TRUE)));
                      break;

                    case 'touchThreshold':
                      $states = array('visible' => array(':input[name*="[' . $i . '][settings][touchMove]"]' => array('checked' => TRUE)));
                      break;

                    case 'swipeToSlide':
                      $states = array('visible' => array(':input[name*="[' . $i . '][settings][swipe]"]' => array('checked' => TRUE)));
                      break;

                    case 'cssEase':
                    case 'cssEaseOverride':
                      $states = array('visible' => array(':input[name*="[' . $i . '][settings][useCSS]"]' => array('checked' => TRUE)));
                      break;
                  }

                  if ($states) {
                    $form['options']['responsives']['responsive'][$i][$key][$k]['#states'] = $states;
                  }
                }
                if (isset($item['options'])) {
                  $form['options']['responsives']['responsive'][$i][$key][$k]['#options'] = $item['options'];
                }
                if (isset($item['empty_option'])) {
                  $form['options']['responsives']['responsive'][$i][$key][$k]['#empty_option'] = $item['empty_option'];
                }
                if (isset($item['field_suffix'])) {
                  $form['options']['responsives']['responsive'][$i][$key][$k]['#field_suffix'] = $item['field_suffix'];
                }
                if (!isset($item['field_suffix']) && $item['cast'] == 'bool') {
                  $form['options']['responsives']['responsive'][$i][$key][$k]['#field_suffix'] = '';
                  $form['options']['responsives']['responsive'][$i][$key][$k]['#title_display'] = 'before';
                }
              }
              break;

            default:
              break;
          }
        }
      }
    }
  }

  /**
   * Overrides the edit form submit handler.
   *
   * At this point, submission is successful. Our only responsibility is
   * to copy anything out of values onto the item that we are able to edit.
   *
   * If the keys all match up to the schema, this method will not need to be
   * overridden.
   */
  function edit_form_submit(&$form, &$form_state) {
    parent::edit_form_submit($form, $form_state);

    // Map and update the friendly CSS easing to its bezier equivalent.
    $options = isset($form_state['values']['options']) ? $form_state['values']['options'] : array();
    if(isset($options['settings']['cssEaseOverride'])) {
      $override = $options['settings']['cssEaseOverride'] ? _slick_css_easing_mapping($options['settings']['cssEaseOverride']) : '';
      $form_state['item']->options['settings']['cssEaseBezier'] = $override;
    }

    if (isset($options['responsives']['responsive'])) {
      foreach ($options['responsives']['responsive'] as $key => $responsive) {
        if (isset($responsive['settings']['cssEaseOverride'])) {
          $responsive_override = $responsive['settings']['cssEaseOverride'] ? _slick_css_easing_mapping($responsive['settings']['cssEaseOverride']) : '';
          $form_state['item']->options['responsives']['responsive'][$key]['settings']['cssEaseBezier'] = $responsive_override;
        }
      }
    }
  }

  /**
   * Defines a list of form elements available for the Slick.
   *
   * @return array
   *   All available Slick form elements.
   *
   * @see http://kenwheeler.github.io/slick
   */
  public function getSlickElements() {
    $options = &drupal_static(__METHOD__, NULL);

    if (!isset($options)) {
      $options = array();

      $options['mobileFirst'] = array(
        'title' => t('Mobile first'),
        'description' => t('Responsive settings use mobile first calculation.'),
        'type' => 'checkbox',
      );

      $options['asNavFor'] = array(
        'title' => t('asNavFor target'),
        'description' => t('Leave empty if using sub-modules to have it auto-matched. Set the slider to be the navigation of other slider (Class or ID Name). Use selector identifier ("." or "#") accordingly. If class, use the provided Wrapper class under General as needed, e.g.: if the main display has class "slick--for", and the thumbnail navigation "slick--nav", place the opposite here as its target. Or use existing classes based on optionsets, e.g.: .slick--optionset--main, or .slick--optionset--main-nav. Overriden per field formatter.'),
        'type' => 'textfield',
      );

      $options['accessibility'] = array(
        'title' => t('Accessibility'),
        'description' => t('Enables tabbing and arrow key navigation.'),
        'type' => 'checkbox',
      );

      $options['adaptiveHeight'] = array(
        'title' => t('Adaptive height'),
        'description' => t('Enables adaptive height for single slide horizontal carousels.'),
        'type' => 'checkbox',
      );

      $options['autoplay'] = array(
        'title' => t('Autoplay'),
        'description' => t('Enables autoplay.'),
        'type' => 'checkbox',
      );

      $options['autoplaySpeed'] = array(
        'title' => t('Autoplay speed'),
        'description' => t('Autoplay speed in milliseconds.'),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
      );

      $options['pauseOnHover'] = array(
        'title' => t('Pause on hover'),
        'description' => t('Pause autoplay on hover.'),
        'type' => 'checkbox',
        'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
      );

      $options['pauseOnDotsHover'] = array(
        'title' => t('Pause on dots hover'),
        'description' => t('Pauses autoplay when a dot is hovered.'),
        'type' => 'checkbox',
        'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
      );

      $options['arrows'] = array(
        'title' => t('Arrows'),
        'description' => t('Show prev/next arrows'),
        'type' => 'checkbox',
      );

      $options['appendArrows'] = array(
        'title' => t('Append arrows'),
        'description' => t("Change where the navigation arrows are attached (Selector, htmlString, Array, Element, jQuery object). Leave it to default to wrap it within .slick__arrow container, otherwise change its markups accordingly."),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
      );

      $options['prevArrow'] = array(
        'title' => t('Previous arrow'),
        'description' => t("Customize the previous arrow markups. Make sure to keep the expected class."),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
      );

      $options['nextArrow'] = array(
        'title' => t('Next arrow'),
        'description' => t("Customize the next arrow markups. Make sure to keep the expected class."),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
      );

      $options['centerMode'] = array(
        'title' => t('Center mode'),
        'description' => t('Enables centered view with partial prev/next slides. Use with odd numbered slidesToShow counts.'),
        'type' => 'checkbox',
      );

      $options['centerPadding'] = array(
        'title' => t('Center padding'),
        'description' => t('Side padding when in center mode (px or %). Be aware, too large padding at small breakpoint will screw the slide calculation with slidesToShow.'),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][centerMode]"]' => array('checked' => TRUE))),
      );

      $options['dots'] = array(
        'title' => t('Dots'),
        'description' => t('Show dot indicators.'),
        'type' => 'checkbox',
      );

      $options['dotsClass'] = array(
        'title' => t('Dot class'),
        'description' => t('Class for slide indicator dots container. Do not prefix with dot. If you change this, edit its CSS accordingly.'),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
      );

      $options['appendDots'] = array(
        'title' => t('Append dots'),
        'description' => t('Change where the navigation dots are attached (Selector, htmlString, Array, Element, jQuery object). If you change this, make sure to provide its relevant markup.'),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
      );

      $options['draggable'] = array(
        'title' => t('Draggable'),
        'description' => t('Enable mouse dragging.'),
        'type' => 'checkbox',
      );

      $options['fade'] = array(
        'title' => t('Fade'),
        'description' => t('Enable fade'),
        'type' => 'checkbox',
      );

      $options['focusOnSelect'] = array(
        'title' => t('Focus on select'),
        'description' => t('Enable focus on selected element (click).'),
        'type' => 'checkbox',
      );

      $options['infinite'] = array(
        'title' => t('Infinite'),
        'description' => t('Infinite loop sliding.'),
        'type' => 'checkbox',
      );

      $options['initialSlide'] = array(
        'title' => t('Initial slide'),
        'description' => t('Slide to start on.'),
        'type' => 'textfield',
      );

      $options['lazyLoad'] = array(
        'title' => t('Lazy load'),
        'description' => t("Set lazy loading technique. 'ondemand' will load the image as soon as you slide to it, 'progressive' loads one image after the other when the page loads. Note: dummy image is no good for ondemand. If ondemand fails to generate images, try progressive instead. Or use <a href='@url' target='_blank'>imageinfo_cache</a>. To share images for Pinterest, leave empty, otherwise no way to read actual image src.", array('@url' => '//www.drupal.org/project/imageinfo_cache')),
        'type' => 'select',
        'options' => drupal_map_assoc(array('ondemand', 'progressive')),
        'empty_option' => t('- None -'),
      );

      $options['respondTo'] = array(
        'title' => t('Respond to'),
        'description' => t("Width that responsive object responds to. Can be 'window', 'slider' or 'min' (the smaller of the two)."),
        'type' => 'select',
        'options' => drupal_map_assoc(array('window', 'slider', 'min')),
      );

      $options['rtl'] = array(
        'title' => t('RTL'),
        'description' => t("Change the slider's direction to become right-to-left."),
        'type' => 'checkbox',
      );

      $options['slide'] = array(
        'title' => t('Slide element'),
        'description' => t("Element query to use as slide. Slick will use any direct children as slides, without having to specify which tag or selector to target."),
        'type' => 'textfield',
      );

      $options['slidesToShow'] = array(
        'title' => t('Slides to show'),
        'description' => t('Number of slides to show at a time. If 1, it will behave like slideshow, more than 1 a carousel. Provide more if it is a thumbnail navigation with asNavFor. Only works with odd number slidesToShow counts when using centerMode.'),
        'type' => 'textfield',
      );

      $options['slidesToScroll'] = array(
        'title' => t('Slides to scroll'),
        'description' => t('Number of slides to scroll at a time, or steps at each scroll.'),
        'type' => 'textfield',
      );

      $options['speed'] = array(
        'title' => t('Speed'),
        'description' => t('Slide/Fade animation speed in milliseconds.'),
        'type' => 'textfield',
        'field_suffix' => 'ms',
      );

      $options['swipe'] = array(
        'title' => t('Swipe'),
        'description' => t('Enable swiping.'),
        'type' => 'checkbox',
      );

      $options['swipeToSlide'] = array(
        'title' => t('Swipe to slide'),
        'description' => t('Allow users to drag or swipe directly to a slide irrespective of slidesToScroll.'),
        'type' => 'checkbox',
        'states' => array('visible' => array(':input[name*="options[settings][swipe]"]' => array('checked' => TRUE))),
      );

      $options['edgeFriction'] = array(
        'title' => t('Edge friction'),
        'description' => t("Resistance when swiping edges of non-infinite carousels. If you don't want resistance, set it to 1."),
        'type' => 'textfield',
      );

      $options['touchMove'] = array(
        'title' => t('Touch move'),
        'description' => t('Enable slide motion with touch.'),
        'type' => 'checkbox',
      );

      $options['touchThreshold'] = array(
        'title' => t('Touch threshold'),
        'description' => t('Swipe distance threshold.'),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][touchMove]"]' => array('checked' => TRUE))),
      );

      $options['useCSS'] = array(
        'title' => t('Use CSS'),
        'description' => t('Enable/Disable CSS Transitions.'),
        'type' => 'checkbox',
      );

      $options['cssEase'] = array(
        'title' => t('CSS ease'),
        'description' => t('CSS3 animation easing. <a href="@ceaser">Learn</a> <a href="@bezier">more</a>.', array('@ceaser' => '//matthewlein.com/ceaser/', '@bezier' => '//cubic-bezier.com')),
        'type' => 'textfield',
        'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
      );

      $options['cssEaseBezier'] = array(
        'type' => 'hidden',
      );

      $options['cssEaseOverride'] = array(
        'title' => t('CSS ease override'),
        'description' => t('If provided, this will override the CSS ease with the pre-defined CSS easings based on <a href="@ceaser">CSS Easing Animation Tool</a>. Leave it empty to use your own CSS ease.', array('@ceaser' => '//matthewlein.com/ceaser/')),
        'type' => 'select',
        'options' => _slick_css_easing_options(),
        'empty_option' => t('- None -'),
        'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
      );

      $options['easing'] = array(
        'title' => t('jQuery easing'),
        'description' => t('Add easing for jQuery animate as fallback. Use with <a href="@easing">easing</a> libraries or default easing methods. Optionally install <a href="@jqeasing">jqeasing module</a>. This will be ignored and replaced by CSS ease for supporting browsers, or effective if useCSS is disabled.', array('@jqeasing' => '//drupal.org/project/jqeasing', '@easing' => '//gsgd.co.uk/sandbox/jquery/easing/')),
        'type' => 'select',
        'options' => _slick_easing_options(),
        'empty_option' => t('- None -'),
      );

      $options['variableWidth'] = array(
        'title' => t('variableWidth'),
        'description' => t('Disables automatic slide width calculation.'),
        'type' => 'checkbox',
      );

      $options['vertical'] = array(
        'title' => t('Vertical'),
        'description' => t('Vertical slide direction.'),
        'type' => 'checkbox',
      );

      $options['waitForAnimate'] = array(
        'title' => t('waitForAnimate'),
        'description' => t('Ignores requests to advance the slide while animating.'),
        'type' => 'checkbox',
      );

      // Clone the default values from slick.elements.inc.
      $slick_options = slick_get_options();
      foreach ($slick_options as $name => $option) {
        $options[$name]['default'] = $option['default'];
        $options[$name]['cast'] = $option['cast'];
      }
    }
    return $options;
  }

  /**
   * Defines available options for the responsive Slick.
   *
   * @param $count
   *   The number of breakpoints.
   *
   * @return array
   *   An array of Slick responsive options.
   */
  public function getSlickResponsiveElements($count = 0) {
    $options = array();

    $breakpoints = drupal_map_assoc(range(0, ($count - 1)));
    $slick_elements = slick_clean_options($this->getSlickElements());

    foreach ($breakpoints as $key => $breakpoint) {
      $options[$key] = array(
        'title' => t('Breakpoint #@key', array('@key' => ($key + 1))),
        'type' => 'fieldset',
      );

      $options[$key]['breakpoint'] = array(
        'title' => t('Breakpoint'),
        'description' => t('Breakpoint width in pixel.'),
        'type' => 'textfield',
        'cast' => 'int',
        'field_suffix' => 'px',
        'default' => FALSE,
      );

      $options[$key]['unslick'] = array(
        'title' => t('Unslick'),
        'description' => t("Disable Slick at a given breakpoint. Note, you can't window shrink this, once you unslick, you are unslicked."),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => '',
      );

      $options[$key]['settings'] = array();

      // Duplicate relevant main settings.
      foreach ($slick_elements as $name => $element) {
        $options[$key]['settings'][$name] = $element;
      }
    }
    return $options;
  }

}

/**
 * Callback for ajax-enabled breakpoints textfield, no method allowed for D7.
 *
 * Selects and returns the responsive options.
 */
function slick_add_breakpoints_ajax_callback($form, $form_state) {
  if ($form_state['values']['breakpoints'] && $form_state['values']['breakpoints'] >= 8) {
    drupal_set_message(t('You are trying to load too many Breakpoints. Try reducing it to reasonable numbers say, between 1 to 5.'));
  }
  return $form['options']['responsives']['responsive'];
}
