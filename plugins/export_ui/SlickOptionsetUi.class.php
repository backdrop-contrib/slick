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
    $slick_options = slick_get_options();
    $form['options']['settings'] = array(
      '#title' => t('Settings'),
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#attributes' => array('class' => array('fieldset--settings', 'has-tooltip')),
    );

    foreach ($slick_options as $name => $setting) {
      $default_value = isset($options['settings'][$name]) ? $options['settings'][$name] : $setting['default'];
      if ($name == 'cssEaseBezier' && ($override = $options['settings']['cssEaseOverride']) !== '') {
        // @todo make this function internal to this form.
        $default_value = _slick_css_easing_mapping($override);
      }
      $form['options']['settings'][$name] = array(
        '#title' => isset($setting['title']) ? $setting['title'] : '',
        '#description' => isset($setting['description']) ? $setting['description'] : '',
        '#type' => $setting['type'],
        '#default_value' => $default_value,
        '#attributes' => array('class' => array('is-tooltip')),
      );

      if (isset($setting['field_suffix'])) {
        $form['options']['settings'][$name]['#field_suffix'] = $setting['field_suffix'];
      }

      if ($setting['type'] == 'textfield') {
        $form['options']['settings'][$name]['#size'] = 20;
        $form['options']['settings'][$name]['#maxlength'] = 255;
      }

      if (!isset($setting['field_suffix']) && $setting['cast'] == 'bool') {
        $form['options']['settings'][$name]['#field_suffix'] = '';
        $form['options']['settings'][$name]['#title_display'] = 'before';
      }

      if ($setting['cast'] == 'int') {
        $form['options']['settings'][$name]['#maxlength'] = 60;
        $form['options']['settings'][$name]['#attributes']['class'][] = 'form-text--int';
      }

      if (isset($setting['states'])) {
        $form['options']['settings'][$name]['#states'] = $setting['states'];
      }

      if (isset($setting['options'])) {
        $form['options']['settings'][$name]['#options'] = $setting['options'];
      }

      if (isset($setting['empty_option'])) {
        $form['options']['settings'][$name]['#empty_option'] = $setting['empty_option'];
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
      $slick_options = slick_get_responsive_options($form_state['breakpoints_count']);

      foreach ($slick_options as $i => $responsives) {
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

}

/**
 * Callback for ajax-enabled breakpoint textfield.
 *
 * Selects and returns the fieldset with the names in it.
 */
function slick_add_breakpoints_ajax_callback($form, $form_state) {
  if ($form_state['values']['breakpoints'] && $form_state['values']['breakpoints'] >= 8) {
    drupal_set_message(t('You are trying to load too many Breakpoints. Try reducing it to reasonable numbers say, between 1 to 5.'));
  }
  return $form['options']['responsives']['responsive'];
}
