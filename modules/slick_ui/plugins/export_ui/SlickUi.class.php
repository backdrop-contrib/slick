<?php

/**
 * @file
 * Contains the CTools export UI integration code.
 */

/**
 * CTools Export UI class handler for Slick UI.
 */
class SlickUi extends ctools_export_ui {



  public function slick_ui_list_build_row($item, &$form_state, $operations) {
    parent::list_build_row($item, $form_state, $operations);

    $name = $item->{$this->plugin['export']['key']};
    $skins = slick_skins();
    $breakpoints = $this->items[$name]->breakpoints ? $this->items[$name]->breakpoints : 0;
    $skin = $this->items[$name]->skin;
    $skin_name = $skin ? check_plain($skin) : t('None');

    if ($skin) {
      $description = isset($skins[$skin]['description']) && $skins[$skin]['description'] ? filter_xss($skins[$skin]['description']) : '';
      if ($description) {
        $skin_name .= '<br /><em>' . $description . '</em>';
      }
    }

    $breakpoints_row[] = array(
      'data' => $breakpoints,
      'class' => array('ctools-export-ui-breakpoints'),
    );
    array_splice($this->rows[$name]['data'], 2, 0, $breakpoints_row);

    $skin_row[] = array(
      'data' => $skin_name,
      'class' => array('ctools-export-ui-skin'),
      'style' => "white-space: normal; word-wrap: break-word; max-width: 320px;",
    );
    array_splice($this->rows[$name]['data'], 3, 0, $skin_row);
  }

  /**
   * Overrides parent::list_table_header.
   */
  public function list_table_header() {
    $headers = parent::list_table_header();

    $breakpoints_header[] = array(
      'data' => t('Breakpoint'),
      'class' => array('ctools-export-ui-breakpoints'));
    array_splice($headers, 2, 0, $breakpoints_header);

    $skin_header[] = array(
      'data' => t('Skin'),
      'class' => array('ctools-export-ui-skin'));
    array_splice($headers, 3, 0, $skin_header);

    return $headers;
  }

  /**
   * Overrides parent::build_operations.
   */
  public function build_operations($item) {
    $allowed_operations = parent::build_operations($item);

    if ($item->name == 'default') {
      if (isset($allowed_operations['enable'])) {
        unset($allowed_operations['enable']);
      }
      if (isset($allowed_operations['edit'])) {
        unset($allowed_operations['edit']);
      }
      if (isset($allowed_operations['disable'])) {
        unset($allowed_operations['disable']);
      }
    }

    return $allowed_operations;
  }

}
