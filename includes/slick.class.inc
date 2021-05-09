<?php
/**
 * @file
 * Defines the SlickOptionset class.
 */

/**
 * Defines a base entity class.
 */
class SlickOptionset extends stdClass {

  /**
   * Machine name for the optionset.
   */
  public $name;

  /**
   * Human readable name for the option set.
   */
  public $label;

  /**
   * Skin selected for this optionset.
   */
  public $skin;

  /**
   * Breakpoints selected for this optionset.
   */
  public $breakpoints;

  /**
   * Options selected for this optionset.
   */
  public $options;

  /**
   * Enabled or disabled.
   */
  public $status;

  /**
   * Whether the optionset has been saved yet or not.
   */
  public $is_new = FALSE;

  /**
   * Determines if the optionset is new or not.
   */
  public function isNew() {
    return !empty($this->is_new);
  }

  /**
   * Save the Optionset to config.
   */
  public function save() {
    if (empty($this->options)) {
      $this->options = array();
    }
    if (!isset($this->status)) {
      $this->status = TRUE;
    }
    $values = array(
      'name' => $this->name,
      'label' => $this->label,
      'skin' => $this->skin,
      'breakpoints' => $this->breakpoints,
      'options' => $this->options,
      'status' => $this->status,
    );
    $config = config('slick.optionset.' . $this->name);
    $config->setData($values);
    $config->save($config);
  }

  /**
   * Delete the config for this optionset.
   */
  public function delete() {
    if (!$this->isNew()) {
      $config = config_get('slick.optionset.'. $this->name);
      $config->delete();
    }
  }
}
