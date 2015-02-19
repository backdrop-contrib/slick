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
    $image_styles = image_style_options(FALSE);
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

    foreach ($slick_options as $name => $values) {
      $form['options']['settings'][$name] = array(
        '#title' => $values['title'],
        '#description' => $values['description'],
        '#type' => $values['type'],
        '#default_value' => isset($options['settings'][$name]) ? $options['settings'][$name] : $values['default'],
        '#attributes' => array('class' => array('is-tooltip')),
      );

      if (isset($values['field_suffix'])) {
        $form['options']['settings'][$name]['#field_suffix'] = $values['field_suffix'];
      }

      if ($values['type'] == 'textfield') {
        $form['options']['settings'][$name]['#size'] = 20;
        $form['options']['settings'][$name]['#maxlength'] = 255;
      }

      if (!isset($values['field_suffix']) && $values['cast'] == 'bool') {
        $form['options']['settings'][$name]['#field_suffix'] = '';
        $form['options']['settings'][$name]['#title_display'] = 'before';
      }

      if ($values['cast'] == 'int') {
        $form['options']['settings'][$name]['#maxlength'] = 60;
        $form['options']['settings'][$name]['#attributes']['class'][] = 'form-text--int';
      }

      if (isset($values['states'])) {
        $form['options']['settings'][$name]['#states'] = $values['states'];
      }

      if (isset($values['options'])) {
        $form['options']['settings'][$name]['#options'] = $values['options'];
      }

      if (isset($values['empty_option'])) {
        $form['options']['settings'][$name]['#empty_option'] = $values['empty_option'];
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
      '#description' => t('Containing breakpoints and settings objects. Settings set at a given breakpoint/screen width is self-contained and does not inherit the main settings, but defaults. Currently only supports Desktop first: starts breakpoint from the largest to smallest. Starting from 1.4, Slick supports mobile first.'),
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
      foreach ($slick_options as $i => $values) {
        if ($values['type'] == 'fieldset') {
          $fieldset_class = drupal_clean_css_identifier(drupal_strtolower($values['title']));
          // Invidual breakpoint fieldset.
          $form['options']['responsives']['responsive'][$i] = array(
            '#title' => $values['title'],
            '#type' => $values['type'],
            '#description' => isset($values['description']) ? $values['description'] : '',
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#attributes' => array('class' => array('fieldset--responsive', 'fieldset--' . $fieldset_class, 'has-tooltip')),
          );

          foreach ($values as $key => $vals) {
            switch ($key) {
              case 'breakpoint':
              case 'unslick':
                $form['options']['responsives']['responsive'][$i][$key] = array(
                  '#title' => $vals['title'],
                  '#description' => $vals['description'],
                  '#type' => $vals['type'],
                  '#default_value' => isset($options['responsives']['responsive'][$i][$key]) ? $options['responsives']['responsive'][$i][$key] : $vals['default'],
                  '#attributes' => array('class' => array('is-tooltip')),
                );

                if ($vals['type'] == 'textfield') {
                  $form['options']['responsives']['responsive'][$i][$key]['#size'] = 20;
                  $form['options']['responsives']['responsive'][$i][$key]['#maxlength'] = 255;
                }
                if ($vals['cast'] == 'int') {
                  $form['options']['responsives']['responsive'][$i][$key]['#maxlength'] = 60;
                }
                if (isset($vals['states'])) {
                  $form['options']['responsives']['responsive'][$i][$key]['#states'] = $vals['states'];
                }
                if (isset($vals['options'])) {
                  $form['options']['responsives']['responsive'][$i][$key]['#options'] = $vals['options'];
                }
                if (isset($vals['field_suffix'])) {
                  $form['options']['responsives']['responsive'][$i][$key]['#field_suffix'] = $vals['field_suffix'];
                }
                if (!isset($vals['field_suffix']) && $vals['cast'] == 'bool') {
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
                unset($vals['title']);

                if (is_array($vals)) {
                  foreach ($vals as $k => $value) {
                    if ($value && is_array($value)) {
                      $form['options']['responsives']['responsive'][$i][$key][$k] = array(
                        '#title' => isset($value['title']) ? $value['title'] : '',
                        '#description' => isset($value['description']) ? $value['description'] : '',
                        '#type' => $value['type'],
                        '#attributes' => array('class' => array('is-tooltip')),
                      );
                      if ($value['type'] != 'fieldset') {
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#default_value'] = isset($options['responsives']['responsive'][$i][$key][$k]) ? $options['responsives']['responsive'][$i][$key][$k] : $value['default'];
                      }
                      if (isset($value['states'])) {
                        // Specify proper states for the breakpoint elements.
                        $states = '';
                        switch ($k) {
                          case 'pauseOnHover':
                          case 'pauseOnDotsHover':
                          case 'autoplaySpeed':
                            $states = array('visible' => array(':input[name*="options[responsives][responsive][' . $i . '][settings][autoplay]"]' => array('checked' => TRUE)));
                            break;

                          case 'centerPadding':
                            $states = array('visible' => array(':input[name*="options[responsives][responsive][' . $i . '][settings][centerMode]"]' => array('checked' => TRUE)));
                            break;

                          case 'touchThreshold':
                            $states = array('visible' => array(':input[name*="options[responsives][responsive][' . $i . '][settings][touchMove]"]' => array('checked' => TRUE)));
                            break;

                          case 'swipeToSlide':
                            $states = array('visible' => array(':input[name*="options[responsives][responsive][' . $i . '][settings][swipe]"]' => array('checked' => TRUE)));
                            break;

                          case 'cssEase':
                          case 'cssEaseOverride':
                            $states = array('visible' => array(':input[name*="options[responsives][responsive][' . $i . '][settings][useCSS]"]' => array('checked' => TRUE)));
                            break;
                        }

                        if ($states) {
                          $form['options']['responsives']['responsive'][$i][$key][$k]['#states'] = $states;
                        }
                      }
                      if (isset($value['options'])) {
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#options'] = $value['options'];
                      }
                      if (isset($value['empty_option'])) {
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#empty_option'] = $value['empty_option'];
                      }
                      if (isset($value['field_suffix'])) {
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#field_suffix'] = $value['field_suffix'];
                      }
                      if (!isset($value['field_suffix']) && $value['cast'] == 'bool') {
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#field_suffix'] = '';
                        $form['options']['responsives']['responsive'][$i][$key][$k]['#title_display'] = 'before';
                      }
                    }
                  }
                }
                break;
            }
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
