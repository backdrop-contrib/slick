<?php

namespace Drupal\slick_ui\Form;

use Drupal\slick\Entity\Slick;
use Drupal\slick\Form\SlickAdmin;
use Drupal\slick\SlickManager;
use Drupal\slick_ui\Controller\SlickListBuilder;
use ctools_export_ui;

/**
 * Provides base form for a slick instance configuration form.
 *
 * @todo use Slick methods once stdClass to Slick conversion resolved.
 */
abstract class SlickFormBase extends ctools_export_ui {

  use SlickListBuilder;

  /**
   * The slick admin service.
   *
   * @var \Drupal\slick\Form\SlickAdminInterface
   */
  protected $admin;

  /**
   * The slick manager service.
   *
   * @var \Drupal\slick\SlickManagerInterface
   */
  protected $manager;

  /**
   * The JS easing options.
   *
   * @var array
   */
  protected $jsEasingOptions;

  /**
   * Fake constructor.
   */
  public function init($plugin) {
    parent::init($plugin);

    // No DI as we have no way to initiliaze this class at
    // `ctools_export_ui_get_handler()` at ctools/includes/export-ui.inc.
    $this->manager = new SlickManager();
    $this->admin = new SlickAdmin($this->manager);
  }

  /**
   * Returns the slick admin service.
   */
  public function admin() {
    return $this->admin;
  }

  /**
   * Returns the slick manager service.
   */
  public function manager() {
    return $this->manager;
  }

  /**
   * {@inheritdoc}
   */
  public function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    $slick          = $form_state['item'];
    $path           = drupal_get_path('module', 'slick');
    $tooltip        = ['class' => ['is-tooltip']];
    $tooltip_bottom = $tooltip + ['data-blazy-tooltip' => 'wide', 'data-blazy-tooltip-direction' => 'bottom'];
    $readme         = url($path . '/README.txt');
    $admin_css      = $this->manager->config('admin_css', TRUE, 'blazy.settings');

    $form['#attributes']['class'][] = 'form--slick';
    $form['#attributes']['class'][] = 'form--blazy';
    $form['#attributes']['class'][] = 'form--optionset has-tooltip';

    $form['info']['label']['#attributes']['class'][] = 'is-tooltip';
    $form['info']['name']['#attributes']['class'][] = 'is-tooltip';
    $form['info']['label']['#prefix'] = '<div class="form__header-container clearfix"><div class="form__header form__half form__half--first has-tooltip clearfix">';
    $form['info']['name']['#suffix'] = '</div>';

    $form['skin'] = [
      '#type'          => 'select',
      '#title'         => t('Skin'),
      '#options'       => $this->admin->getSkinsByGroupOptions(),
      '#empty_option'  => t('- None -'),
      '#default_value' => isset($form_state['values']['skin']) ? $form_state['values']['skin'] : $slick->skin,
      '#description'   => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get yourself dirty. See main <a href="@url">README</a> for details on Skins. Only useful for custom work, and ignored/overridden by slick formatters or sub-modules.', ['@url' => $readme]),
      '#attributes'    => $tooltip_bottom,
      '#prefix'        => '<div class="form__header form__half form__half--last has-tooltip clearfix">',
    ];

    $collection = isset($slick->collection) ? $slick->collection : '';
    $form['collection'] = [
      '#type'          => 'select',
      '#title'         => t('Collection'),
      '#options'       => [
        'main'      => t('Main'),
        'thumbnail' => t('Thumbnail'),
      ],
      '#empty_option'  => t('- None -'),
      '#default_value' => isset($form_state['values']['collection']) ? $form_state['values']['collection'] : $collection,
      '#description'   => t('Group this optionset to avoid confusion for optionset selections. Leave empty to make it available for all.'),
      '#attributes'    => $tooltip_bottom,
    ];

    $form['breakpoints'] = [
      '#title'         => t('Breakpoints'),
      '#type'          => 'textfield',
      '#default_value' => isset($form_state['values']['breakpoints']) ? $form_state['values']['breakpoints'] : $slick->breakpoints,
      '#description'   => t('The number of breakpoints added to Responsive display, max 9. This is not Breakpoint Width (480px, etc).'),
      '#ajax' => [
        'callback' => 'slick_ui_add_breakpoints',
        'wrapper'  => 'edit-breakpoints-ajax-wrapper',
        'event'    => 'blur',
      ],
      '#attributes' => $tooltip_bottom,
      '#maxlength'  => 1,
    ];

    $optimized = isset($slick->optimized) ? $slick->optimized : '';
    $form['optimized'] = [
      '#type'          => 'checkbox',
      '#title'         => t('Optimized'),
      '#default_value' => isset($form_state['values']['optimized']) ? $form_state['values']['optimized'] : $optimized,
      '#description'   => t('Check to optimize the stored options. Anything similar to defaults will not be stored, except those required by sub-modules and theme_slick(). Like you hand-code/ cherry-pick the needed options, and are smart enough to not repeat defaults, and free up memory. The rest are taken care of by JS. Uncheck only if theme_slick() can not satisfy the needs, and more hand-coded preprocess is needed which is less likely in most cases.'),
      '#access'        => $slick->name != 'default',
      '#attributes'    => $tooltip_bottom,
    ];

    if ($slick->name == 'default') {
      $form['breakpoints']['#suffix'] = '</div></div>';
    }
    else {
      $form['optimized']['#suffix'] = '</div></div>';
    }

    if ($admin_css) {
      $form['optimized']['#field_suffix'] = '&nbsp;';
      $form['optimized']['#title_display'] = 'before';
    }
  }

  /**
   * {@inheritdoc}
   */
  public function edit_form_submit(&$form, &$form_state) {
    parent::edit_form_submit($form, $form_state);

    // Optimized if so configured.
    $slick = $form_state['item'];
    $default = $slick->name == 'default';
    if ($default) {
      return;
    }

    $defaults = Slick::defaultSettings();
    $required = $this->getOptionsRequiredByTemplate();
    $settings = $form_state['values']['options']['settings'];
    $optimized = $form_state['values']['optimized'];

    // Cast the values.
    $slick->typecast($settings);

    $main_settings = $settings;
    if ($optimized) {
      // Remove wasted dependent options if disabled, empty or not.
      $slick->removeWastedDependentOptions($settings);
      $main = array_diff_assoc($defaults, $required);
      $main_settings = array_diff_assoc($settings, $main);
    }

    $slick->setSettings($main_settings);

    if (isset($form_state['values']['options']['responsives'])
      && $responsives = $form_state['values']['options']['responsives']['responsive']) {
      foreach ($responsives as $delta => &$responsive) {

        settype($responsive['breakpoint'], 'int');
        settype($responsive['unslick'], 'bool');

        if (!empty($responsive['unslick'])) {
          $slick->setResponsiveSettings([], $delta);
        }
        else {
          $slick->typecast($responsive['settings']);

          $responsive_settings = $responsive['settings'];
          if ($optimized) {
            $slick->removeWastedDependentOptions($responsive['settings']);
            $responsive_settings = array_diff_assoc($responsive['settings'], $defaults);
          }

          $slick->setResponsiveSettings($responsive_settings, $delta);
          $slick->setResponsiveSettings($responsive['breakpoint'], $delta, 'breakpoint');
          $slick->setResponsiveSettings($responsive['unslick'], $delta, 'unslick');
        }
      }
    }
  }

  /**
   * List of all easing methods available from jQuery Easing v1.3.
   *
   * @return array
   *   An array of available jQuery Easing options as fallback for browsers that
   *   don't support pure CSS easing.
   */
  public function getJsEasingOptions() {
    if (!isset($this->jsEasingOptions)) {
      $this->jsEasingOptions = [
        'linear'           => 'Linear',
        'swing'            => 'Swing',
        'easeInQuad'       => 'easeInQuad',
        'easeOutQuad'      => 'easeOutQuad',
        'easeInOutQuad'    => 'easeInOutQuad',
        'easeInCubic'      => 'easeInCubic',
        'easeOutCubic'     => 'easeOutCubic',
        'easeInOutCubic'   => 'easeInOutCubic',
        'easeInQuart'      => 'easeInQuart',
        'easeOutQuart'     => 'easeOutQuart',
        'easeInOutQuart'   => 'easeInOutQuart',
        'easeInQuint'      => 'easeInQuint',
        'easeOutQuint'     => 'easeOutQuint',
        'easeInOutQuint'   => 'easeInOutQuint',
        'easeInSine'       => 'easeInSine',
        'easeOutSine'      => 'easeOutSine',
        'easeInOutSine'    => 'easeInOutSine',
        'easeInExpo'       => 'easeInExpo',
        'easeOutExpo'      => 'easeOutExpo',
        'easeInOutExpo'    => 'easeInOutExpo',
        'easeInCirc'       => 'easeInCirc',
        'easeOutCirc'      => 'easeOutCirc',
        'easeInOutCirc'    => 'easeInOutCirc',
        'easeInElastic'    => 'easeInElastic',
        'easeOutElastic'   => 'easeOutElastic',
        'easeInOutElastic' => 'easeInOutElastic',
        'easeInBack'       => 'easeInBack',
        'easeOutBack'      => 'easeOutBack',
        'easeInOutBack'    => 'easeInOutBack',
        'easeInBounce'     => 'easeInBounce',
        'easeOutBounce'    => 'easeOutBounce',
        'easeInOutBounce'  => 'easeInOutBounce',
      ];
    }
    return $this->jsEasingOptions;
  }

  /**
   * List of available CSS easing methods.
   *
   * @param bool $map
   *   Flag to output the array as is for further processing if TRUE.
   *
   * @return array
   *   An array of CSS easings for select options, or all for the mappings.
   *
   * @see https://github.com/kenwheeler/slick/issues/118
   * @see http://matthewlein.com/ceaser/
   * @see http://www.w3.org/TR/css3-transitions/
   */
  public function getCssEasingOptions($map = FALSE) {
    $css_easings = [];
    $available_easings = [

      // Defaults/ Native.
      'ease'           => 'ease|ease',
      'linear'         => 'linear|linear',
      'ease-in'        => 'ease-in|ease-in',
      'ease-out'       => 'ease-out|ease-out',
      'swing'          => 'swing|ease-in-out',

      // Penner Equations (approximated).
      'easeInQuad'     => 'easeInQuad|cubic-bezier(0.550, 0.085, 0.680, 0.530)',
      'easeInCubic'    => 'easeInCubic|cubic-bezier(0.550, 0.055, 0.675, 0.190)',
      'easeInQuart'    => 'easeInQuart|cubic-bezier(0.895, 0.030, 0.685, 0.220)',
      'easeInQuint'    => 'easeInQuint|cubic-bezier(0.755, 0.050, 0.855, 0.060)',
      'easeInSine'     => 'easeInSine|cubic-bezier(0.470, 0.000, 0.745, 0.715)',
      'easeInExpo'     => 'easeInExpo|cubic-bezier(0.950, 0.050, 0.795, 0.035)',
      'easeInCirc'     => 'easeInCirc|cubic-bezier(0.600, 0.040, 0.980, 0.335)',
      'easeInBack'     => 'easeInBack|cubic-bezier(0.600, -0.280, 0.735, 0.045)',
      'easeOutQuad'    => 'easeOutQuad|cubic-bezier(0.250, 0.460, 0.450, 0.940)',
      'easeOutCubic'   => 'easeOutCubic|cubic-bezier(0.215, 0.610, 0.355, 1.000)',
      'easeOutQuart'   => 'easeOutQuart|cubic-bezier(0.165, 0.840, 0.440, 1.000)',
      'easeOutQuint'   => 'easeOutQuint|cubic-bezier(0.230, 1.000, 0.320, 1.000)',
      'easeOutSine'    => 'easeOutSine|cubic-bezier(0.390, 0.575, 0.565, 1.000)',
      'easeOutExpo'    => 'easeOutExpo|cubic-bezier(0.190, 1.000, 0.220, 1.000)',
      'easeOutCirc'    => 'easeOutCirc|cubic-bezier(0.075, 0.820, 0.165, 1.000)',
      'easeOutBack'    => 'easeOutBack|cubic-bezier(0.175, 0.885, 0.320, 1.275)',
      'easeInOutQuad'  => 'easeInOutQuad|cubic-bezier(0.455, 0.030, 0.515, 0.955)',
      'easeInOutCubic' => 'easeInOutCubic|cubic-bezier(0.645, 0.045, 0.355, 1.000)',
      'easeInOutQuart' => 'easeInOutQuart|cubic-bezier(0.770, 0.000, 0.175, 1.000)',
      'easeInOutQuint' => 'easeInOutQuint|cubic-bezier(0.860, 0.000, 0.070, 1.000)',
      'easeInOutSine'  => 'easeInOutSine|cubic-bezier(0.445, 0.050, 0.550, 0.950)',
      'easeInOutExpo'  => 'easeInOutExpo|cubic-bezier(1.000, 0.000, 0.000, 1.000)',
      'easeInOutCirc'  => 'easeInOutCirc|cubic-bezier(0.785, 0.135, 0.150, 0.860)',
      'easeInOutBack'  => 'easeInOutBack|cubic-bezier(0.680, -0.550, 0.265, 1.550)',
    ];

    foreach ($available_easings as $key => $easing) {
      list($readable_easing, $css_easing) = array_pad(array_map('trim', explode("|", $easing, 2)), 2, NULL);
      $css_easings[$key] = $map ? $easing : $readable_easing;
      unset($css_easing);
    }
    return $css_easings;
  }

  /**
   * Defines options required by theme_slick(), used with optimized option.
   */
  public function getOptionsRequiredByTemplate() {
    $options = [
      'lazyLoad'     => 'ondemand',
      'slidesToShow' => 1,
    ];

    drupal_alter('slick_options_required_by_template', $options);
    return $options;
  }

  /**
   * Maps existing jQuery easing value to equivalent CSS easing methods.
   *
   * @param string $easing
   *   The name of the human readable easing.
   *
   * @return string
   *   A string of unfriendly bezier equivalent, or NULL.
   */
  public function getBezier($easing = NULL) {
    $css_easing = '';
    if ($easing) {
      $easings = $this->getCssEasingOptions(TRUE);
      list($readable_easing, $bezier) = array_pad(array_map('trim', explode("|", $easings[$easing], 2)), 2, NULL);
      $css_easing = $bezier;
      unset($readable_easing);
    }
    return $css_easing;
  }

}
