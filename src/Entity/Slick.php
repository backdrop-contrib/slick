<?php

namespace Drupal\slick\Entity;

use Drupal\blazy\Utility\NestedArray;

/**
 * Defines the Slick configuration entity.
 *
 * @todo change back public to protected on succesful update, or keep it.
 */
class Slick implements SlickInterface {

  /**
   * Defines slick table name.
   */
  const TABLE = 'slick_optionset';

  /**
   * The legacy CTools ID for the configurable optionset.
   *
   * @var string
   */
  public $name;

  /**
   * The human-readable name for the optionset.
   *
   * @var string
   */
  public $label;

  /**
   * The optionset group for easy selections.
   *
   * @var string
   */
  public $collection = '';

  /**
   * The skin name for the optionset.
   *
   * @var string
   */
  public $skin = '';

  /**
   * The number of breakpoints for the optionset.
   *
   * @var int
   */
  public $breakpoints = 0;

  /**
   * The flag indicating to optimize the stored options by removing defaults.
   *
   * @var bool
   */
  public $optimized = 0;

  /**
   * The plugin instance options.
   *
   * @var array
   */
  public $options = [];

  /**
   * The plugin default settings.
   *
   * @var array
   */
  protected static $defaultSettings;

  /**
   * Overrides Drupal\Core\Entity\Entity::id().
   */
  public function id() {
    return $this->name;
  }

  /**
   * The slick label.
   *
   * @var string
   */
  public function label() {
    return $this->label;
  }

  /**
   * {@inheritdoc}
   */
  public function getSkin() {
    return $this->skin;
  }

  /**
   * {@inheritdoc}
   */
  public function getBreakpoints() {
    return $this->breakpoints;
  }

  /**
   * {@inheritdoc}
   */
  public function getCollection() {
    return $this->collection;
  }

  /**
   * {@inheritdoc}
   */
  public function optimized() {
    return $this->optimized;
  }

  /**
   * Constructs a Slick instance.
   */
  public function __construct() {
    ctools_include('export');
  }

  /**
   * {@inheritdoc}
   */
  public static function load($id = 'default') {
    ctools_include('export');
    $optionset = ctools_export_crud_load(static::TABLE, $id);

    // Ensures deleted optionset while being used doesn't screw up.
    if (!isset($optionset->name)) {
      $optionset = ctools_export_crud_load(static::TABLE, 'default');
    }

    // @todo remove BC layer which uses stdClass().
    // Slick 2.x was exported as stdClass, convert into Slick.
    if (!($optionset instanceof Slick)) {
      $optionset = self::create((array) $optionset);
    }
    return $optionset;
  }

  /**
   * {@inheritdoc}
   */
  public static function loadMultiple($reset = FALSE) {
    ctools_include('export');
    return ctools_export_crud_load_all(static::TABLE, $reset);
  }

  /**
   * {@inheritdoc}
   */
  public static function exists($name) {
    ctools_include('export');
    $optionset = ctools_export_crud_load('slick_optionset', $name);
    return isset($optionset->name);
  }

  /**
   * {@inheritdoc}
   */
  public static function create(array $values = []) {
    ctools_include('export');

    $optionset = ctools_export_crud_new(static::TABLE);

    $optionset->options = $optionset->options['settings'] = [];
    foreach (self::defaultProperties() as $key => $ignore) {
      if (isset($values[$key])) {
        $optionset->{$key} = $values[$key];
      }
    }

    $defaults['settings'] = self::defaultSettings();
    $optionset->options = $optionset->options + $defaults;
    return $optionset;
  }

  /**
   * Saves the given option set to the database.
   *
   * @param object $optionset
   *   The Optionset object.
   * @param bool $new
   *   Set the $new flag if this set has not been written before.
   *
   * @return object
   *   Returns the newly saved object, FALSE otherwise.
   */
  public static function save($optionset, $new = FALSE) {
    // If the machine name is missing or already in use, return an error.
    if (empty($optionset->name) or (FALSE != self::exists($optionset->name) and $new)) {
      return FALSE;
    }

    // Check for an invalid list of options.
    if (isset($optionset->options) and !is_array($optionset->options)) {
      return FALSE;
    }

    // Assumes creating defaults.
    if (!isset($optionset->options)) {
      $optionset->options = [];
    }

    $defaults['settings'] = self::defaultSettings();
    $optionset->options = $optionset->options + $defaults;

    self::typecast($optionset->options['settings']);

    // Prepare the database values.
    $db_values = [
      'name'        => $optionset->name,
      'label'       => isset($optionset->label) ? $optionset->label : $optionset->name,
      'breakpoints' => isset($optionset->breakpoints) ? $optionset->breakpoints : 0,
      'skin'        => isset($optionset->skin) ? $optionset->skin : '',
      'collection'  => isset($optionset->collection) ? $optionset->collection : '',
      'optimized'   => isset($optionset->optimized) ? $optionset->optimized : 0,
      'options'     => $optionset->options,
    ];

    if ($new) {
      $result = drupal_write_record(static::TABLE, $db_values);
    }
    else {
      $result = drupal_write_record(static::TABLE, $db_values, 'name');
    }

    // Return the object if the values were saved successfully.
    if (($new and SAVED_NEW == $result) or (!$new and SAVED_UPDATED == $result)) {
      return $optionset;
    }

    // Otherwise, an error occured.
    return FALSE;
  }

  /**
   * Deletes the given option set from the database.
   *
   * @param string|object $optionset
   *   Optionset object, or string machine name.
   */
  public static function delete($optionset) {
    ctools_include('export');
    $object = is_string($optionset) ? self::load($optionset) : $optionset;

    // This only deletes from the database, which means that if an item is in
    // code, then this is actually a revert.
    ctools_export_crud_delete(static::TABLE, $object);
  }

  /**
   * Returns the typecast values.
   *
   * @param array $settings
   *   An array of Optionset settings.
   */
  public static function typecast(array &$settings = []) {
    if (empty($settings)) {
      return;
    }

    $defaults = self::defaultSettings();
    foreach ($defaults as $name => $value) {
      if (isset($settings[$name])) {
        // Seems double is ignored, and causes a missing schema, unlike float.
        $type = gettype($defaults[$name]);
        $type = $type == 'double' ? 'float' : $type;

        // Change float to integer if value is no longer float.
        if ($name == 'edgeFriction') {
          $type = $settings[$name] == '1' ? 'integer' : 'float';
        }

        settype($settings[$name], $type);
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getOptions($group = NULL, $property = NULL) {
    if ($group) {
      if (is_array($group)) {
        return NestedArray::getValue($this->options, (array) $group);
      }
      elseif (isset($property) && isset($this->options[$group])) {
        return isset($this->options[$group][$property]) ? $this->options[$group][$property] : NULL;
      }
      return $this->options[$group];
    }

    return $this->options;
  }

  /**
   * {@inheritdoc}
   */
  public function getSettings() {
    // With the Optimized options, all defaults are cleaned out, merge em.
    return isset($this->options['settings']) ? array_merge(self::defaultSettings(), $this->options['settings']) : self::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function setSettings(array $settings = []) {
    $this->options['settings'] = $settings;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getSetting($name) {
    return isset($this->getSettings()[$name]) ? $this->getSettings()[$name] : NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function setSetting($name, $value) {
    $this->options['settings'][$name] = $value;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    if (!isset(static::$defaultSettings)) {
      static::$defaultSettings = [
        'mobileFirst'      => FALSE,
        'asNavFor'         => '',
        'accessibility'    => TRUE,
        'adaptiveHeight'   => FALSE,
        'autoplay'         => FALSE,
        'autoplaySpeed'    => 3000,
        'pauseOnHover'     => TRUE,
        'pauseOnDotsHover' => FALSE,
        'arrows'           => TRUE,
        'prevArrow'        => '<button type="button" data-role="none" class="slick-prev" aria-label="Previous" tabindex="0">Previous</button>',
        'nextArrow'        => '<button type="button" data-role="none" class="slick-next" aria-label="Next" tabindex="0">Next</button>',
        'downArrow'        => FALSE,
        'downArrowTarget'  => '',
        'downArrowOffset'  => 0,
        'centerMode'       => FALSE,
        'centerPadding'    => '50px',
        'dots'             => FALSE,
        'dotsClass'        => 'slick-dots',
        'appendDots'       => '$(element)',
        'draggable'        => TRUE,
        'fade'             => FALSE,
        'focusOnSelect'    => FALSE,
        'infinite'         => TRUE,
        'initialSlide'     => 0,
        'lazyLoad'         => 'ondemand',
        'mouseWheel'       => FALSE,
        'randomize'        => FALSE,
        'respondTo'        => 'window',
        'rows'             => 1,
        'slidesPerRow'     => 1,
        'slide'            => '',
        'slidesToShow'     => 1,
        'slidesToScroll'   => 1,
        'speed'            => 500,
        'swipe'            => TRUE,
        'swipeToSlide'     => FALSE,
        'edgeFriction'     => 0.35,
        'touchMove'        => TRUE,
        'touchThreshold'   => 5,
        'useCSS'           => TRUE,
        'cssEase'          => 'ease',
        'cssEaseBezier'    => '',
        'cssEaseOverride'  => '',
        'useTransform'     => TRUE,
        'easing'           => 'linear',
        'variableWidth'    => FALSE,
        'vertical'         => FALSE,
        'verticalSwiping'  => FALSE,
        'waitForAnimate'   => TRUE,
      ];
    }
    return static::$defaultSettings;
  }

  /**
   * Returns default database fields as properties.
   */
  public static function defaultProperties() {
    return [
      'name' => 'default',
      'label' => 'Default',
      'skin' => '',
      'breakpoints' => 0,
      'collection' => '',
      'optimized' => 0,
      'options' => [],
    ];
  }

  /**
   * Returns the Slick responsive settings.
   *
   * @return array
   *   The responsive options.
   */
  public function getResponsiveOptions() {
    if (empty($this->breakpoints)) {
      return FALSE;
    }
    $options = [];
    if (isset($this->options['responsives']['responsive'])) {
      $responsives = $this->options['responsives'];
      if ($responsives['responsive']) {
        foreach ($responsives['responsive'] as $delta => $responsive) {
          if (empty($responsives['responsive'][$delta]['breakpoint'])) {
            unset($responsives['responsive'][$delta]);
          }
          if (isset($responsives['responsive'][$delta])) {
            $options[$delta] = $responsive;
          }
        }
      }
    }
    return $options;
  }

  /**
   * Sets the Slick responsive settings.
   *
   * @return $this
   *   The class instance that this method is called on.
   */
  public function setResponsiveSettings($values, $delta = 0, $key = 'settings') {
    $this->options['responsives']['responsive'][$delta][$key] = $values;
    return $this;
  }

  /**
   * Strip out options containing default values so to have real clean JSON.
   *
   * @return array
   *   The cleaned out settings.
   */
  public function removeDefaultValues(array $js) {
    $config = [];
    $defaults = self::defaultSettings();

    // Remove wasted dependent options if disabled, empty or not.
    $this->removeWastedDependentOptions($js);
    $config = array_diff_assoc($js, $defaults);

    // Remove empty lazyLoad, or left to default ondemand, to avoid JS error.
    if (empty($config['lazyLoad'])) {
      unset($config['lazyLoad']);
    }

    // Do not pass arrows HTML to JSON object as some are enforced.
    $excludes = [
      'downArrow',
      'downArrowTarget',
      'downArrowOffset',
      'prevArrow',
      'nextArrow',
    ];
    foreach ($excludes as $key) {
      unset($config[$key]);
    }

    // Clean up responsive options if similar to defaults.
    if ($responsives = $this->getResponsiveOptions()) {
      $cleaned = [];
      foreach ($responsives as $key => $responsive) {
        $cleaned[$key]['breakpoint'] = $responsives[$key]['breakpoint'];

        // Destroy responsive slick if so configured.
        if (!empty($responsives[$key]['unslick'])) {
          $cleaned[$key]['settings'] = 'unslick';
          unset($responsives[$key]['unslick']);
        }
        else {
          // Remove wasted dependent options if disabled, empty or not.
          $this->removeWastedDependentOptions($responsives[$key]['settings']);
          $cleaned[$key]['settings'] = array_diff_assoc($responsives[$key]['settings'], $defaults);
        }
      }
      $config['responsive'] = $cleaned;
    }
    return $config;
  }

  /**
   * Removes wasted dependent options, even if not empty.
   */
  public function removeWastedDependentOptions(array &$js) {
    foreach (self::getDependentOptions() as $key => $option) {
      if (isset($js[$key]) && empty($js[$key])) {
        foreach ($option as $dependent) {
          unset($js[$dependent]);
        }
      }
    }

    if (!empty($js['useCSS']) && !empty($js['cssEaseBezier'])) {
      $js['cssEase'] = $js['cssEaseBezier'];
    }
    unset($js['cssEaseOverride'], $js['cssEaseBezier']);
  }

  /**
   * Defines the dependent options.
   *
   * @return array
   *   The dependent options.
   */
  public static function getDependentOptions() {
    $down_arrow = ['downArrowTarget', 'downArrowOffset'];
    return [
      'arrows'     => ['prevArrow', 'nextArrow', 'downArrow'] + $down_arrow,
      'downArrow'  => $down_arrow,
      'autoplay'   => ['pauseOnHover', 'pauseOnDotsHover', 'autoplaySpeed'],
      'centerMode' => ['centerPadding'],
      'dots'       => ['dotsClass', 'appendDots'],
      'swipe'      => ['swipeToSlide'],
      'useCSS'     => ['cssEase', 'cssEaseBezier', 'cssEaseOverride'],
      'vertical'   => ['verticalSwiping'],
    ];
  }

}
