<?php

function swab_advice_group_page($form, &$form_state) {
  module_load_include('inc', 'swab_advice_group', 'swab_advice_group_forms');

  drupal_add_js(drupal_get_path('module', 'swab_advice_group') . '/js/swab_advice_group.js');

  $selectors = array_filter(explode("\n", str_replace("\r", "\n", trim(variable_get('improved_multi_select:selectors', '')))));
  $replace_all = variable_get('improved_multi_select:isall', FALSE);
  $selectors = improved_multi_select_load_selectors($replace_all, $selectors);
  drupal_add_css(drupal_get_path('module', 'improved_multi_select') . '/improved_multi_select.css');
  drupal_add_js(drupal_get_path('module', 'improved_multi_select') . '/improved_multi_select.js');
  drupal_add_js(array('improved_multi_select' => array('selectors' => $selectors)), 'setting');

  $args = arg();
  switch ($args[1]) {
    case 'ind':
      $field = swab_advice_group_field_get_items('field_advc_indications');
      $form['indications'] = array(
        '#type' => 'select',
        '#title' => t('Indications'),
        '#multiple' => TRUE,
        '#options' => $field['values'],
        '#title_display' => 'invisible'
      );
      $form['row'] = array(
        '#type' => 'hidden',
        '#value' => $args[2],
      );
      $form['save'] = array(
        '#type' => 'button',
        '#value' => t('Save'),
      );
      break;
  }
  return $form;
}
