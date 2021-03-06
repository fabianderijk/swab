<?php

/**
 * @file
 * The file that creates tha form to add and update advices
 */

/**
 * Build the swab_advice_group_add_advice_form form
 *
 * @return
 *   A form array set for theming by theme_swab_advice_group_add_advice_form()
 */
function swab_advice_group_add_advice_form($form, &$form_state) {
  drupal_add_js(drupal_get_path('module', 'swab_advice_group') . '/js/swab_advice_group_advice_form.js');
  $form['advices']['#tree'] = TRUE;

  $age = swab_advice_group_field_get_items('field_advc_age_ref', 'advice');
  $weight = swab_advice_group_field_get_items('field_advc_weight_ref', 'advice');

  $name = 'new_' . time();
  if (!empty($_SESSION[SWAB_ADVICE_GROUP_SESSION])) {
    foreach ($_SESSION[SWAB_ADVICE_GROUP_SESSION] AS $advice) {
      $n = node_load($advice);
      if (!empty($n) || $advice == $name) {
        $age_default = '';
        $weight_default = '';
        if (!empty($n->field_advc_age_ref)) {
          $age_default = $n->field_advc_age_ref['und'][0]['nid'];
        }
        if (!empty($n->field_advc_weight_ref)) {
          $weight_default = $n->field_advc_weight_ref['und'][0]['nid'];
        }
        $form['advices'][$advice] = array(
          'adv_age' => array(
            '#type' => 'select',
            '#description' => check_plain($age['description']),
            '#options' => $age['values'],
            '#required' => $age['required'],
            '#default_value' => $age_default,
          ),
          'adv_weight' => array(
            '#type' => 'select',
            '#description' => check_plain($weight['description']),
            '#options' => $weight['values'],
            '#required' => $weight['required'],
            '#default_value' => $weight_default,
          ),
          'adv_indication' => array(
            '#markup' => 'Indication placeholder',
          ),
          'adv_remarks' => array(
            '#markup' => 'Remarks placeholder',
          ),
          'adv_dosage' => array(
            '#markup' => 'Dosage placeholder',
          ),
          'adv_score' => array(
            '#markup' => 'Score placeholder',
          ),
          'weight' => array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#default_value' => 0,
            '#delta' => 10,
            '#title-display' => 'invisible',
          ),
        );
      }
    }
  }

  $form['advices'][$name] = array(
    'adv_age' => array(
      '#type' => 'select',
      '#description' => check_plain($age['description']),
      '#options' => $age['values'],
      '#required' => $age['required'],
    ),
    'adv_weight' => array(
      '#type' => 'select',
      '#description' => check_plain($weight['description']),
      '#options' => $weight['values'],
      '#required' => $weight['required'],
    ),
    'adv_indication' => array(
      '#markup' => 'Indication placeholder',
    ),
    'adv_remarks' => array(
      '#markup' => 'Remarks placeholder',
    ),
    'adv_dosage' => array(
      '#markup' => 'Dosage placeholder',
    ),
    'adv_score' => array(
      '#markup' => 'Score placeholder',
    ),
    'weight' => array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#default_value' => 0,
      '#delta' => 10,
      '#title-display' => 'invisible',
    ),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['add_another'] = array( '#type' => 'submit', '#value' => t('Add another'));
  $form['actions']['submit'] = array( '#type' => 'submit', '#value' => t('Save Changes'));
  return $form;
}

/**
 * Theme callback for the swab_advice_group_add_advice_form form
 *
 * The theme callback will format the $form data structure into a table and
 * add our tabledrag functionality.  (Note that drupal_add_tabledrag should be
 * called from the theme layer, and not from a form declaration.  This helps
 * keep template files clean and readable, and prevents tabledrag.js from
 * being added twice accidently.
 *
 * @return
 *   The rendered tabledrag form
 */
function theme_swab_advice_group_add_advice_form($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['advices']) as $id) {
    $form['advices'][$id]['weight']['#attributes']['class'] = array('add-edit-advices-weight');
    $rows[] = array(
      'data' => array(
        drupal_render($form['advices'][$id]['adv_age']),
        drupal_render($form['advices'][$id]['adv_weight']),
        drupal_render($form['advices'][$id]['adv_indication']),
        drupal_render($form['advices'][$id]['adv_remarks']),
        drupal_render($form['advices'][$id]['adv_dosage']),
        drupal_render($form['advices'][$id]['adv_score']),
        drupal_render($form['advices'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $header = array(t('Patient Age'), t('Patient Weight'), t('Indications'), t('Remarks'), t('Dosage Schema'), t('Score'), t('Weight'));
  $table_id = 'add-edit-advices-table';

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);

  drupal_add_tabledrag($table_id, 'order', 'sibling', 'add-edit-advices-weight');
  return $output;
}

/**
 * Submit callback for the swab_advice_group_add_advice_form form
 *
 * Updates the 'weight' column for each element in our table, taking into
 * account that item's new order after the drag and drop actions have been
 * performed. Also updates and adds advices.
 */
function swab_advice_group_add_advice_form_submit() {
  if (isset($_POST)) {
    global $language;
    global $user;
    $advices = array();
    foreach ($_POST['advices'] AS $nid => $adv) {
      if (is_numeric($nid)) { // existing advice
        $n = node_load($nid);
        $n = _swab_advice_group_update_field($n, 'field_advc_age_ref', $n->language, 'nid', $adv['adv_age']);
        $n = _swab_advice_group_update_field($n, 'field_advc_weight_ref', $n->language, 'nid', $adv['adv_weight']);
      }
      elseif ($adv['adv_age'] != '_none' && $adv['adv_weight'] != '_none') { // new advice
        $n = new stdClass();
        $n->nid = NULL;
        $n->type = 'advice';
        node_object_prepare($n);
        $n->language = LANGUAGE_NONE;
        $n->title = 'Advice: ' . format_date($n->created, 'custom', 'd-m-Y H:i:s');
        $n = _swab_advice_group_update_field($n, 'field_advc_age_ref', LANGUAGE_NONE, 'nid', $adv['adv_age']);
        $n = _swab_advice_group_update_field($n, 'field_advc_weight_ref', LANGUAGE_NONE, 'nid', $adv['adv_weight']);
      }
      node_save($n);
      if (!in_array($n->nid, $advices)) {
        $advices[] = $n->nid;
      }
    }
  }
  $_SESSION[SWAB_ADVICE_GROUP_SESSION] = $advices;
  print json_encode(array('success' => 'true'));
}

function _swab_advice_group_update_field($n, $field, $lang, $fieldname, $value) {
  if ($value != '_none') {
    if (!empty($node->{$field})) {
      $n->{$field}[$lang][0][$fieldname] = $value;
    }
    else {
      $n->{$field} = array(
        $lang => array(
          array(
            $fieldname => $value,
          ),
        ),
      );
    }
  }
  return $n;
}

function swab_advice_group_field_get_items($field, $node_type = 'advice_group') {
  $ret = array();

  // get allowed values
  $query = db_select('field_config', 'fc');
  $query
    ->condition('fc.field_name', $field)
    ->fields('fc', array('data'));
  $result = $query->execute();

  foreach ($result AS $row) {
    $data = unserialize($row->data);
    if (isset($data['settings']['allowed_values'][0]['vocabulary'])) { // values come from vocabulary
      $voc = swab_advice_group_get_vocabulary_by_name($data['settings']['allowed_values'][0]['vocabulary']);
      $terms = taxonomy_get_tree($voc->vid);
      foreach ($terms AS $term) {
        $ret['values'][$term->tid] = $term->name;
      }
    }
    elseif (isset($data['settings']['referenceable_types'])) {
      $types = array();
      foreach ($data['settings']['referenceable_types'] AS $type) {
        if (!empty($type)) {
          $types[] = $type;
        }
      }
      $query = db_select('node', 'n');
      $query
        ->condition('n.type', $types, 'IN')
        ->fields('n', array('title', 'nid'));
      $nodes = $query->execute();
      foreach ($nodes AS $node) {
        $ret['values'][$node->nid] = $node->title;
      }
    }
    else {
      $ret['values'] = $data['settings']['allowed_values'];
    }
    break;
  }

  // get widget settings like label & description
  $query = db_select('field_config_instance', 'fci');
  $query
    ->condition('fci.field_name', $field)
    ->condition('fci.bundle', $node_type)
    ->fields('fci', array('data'));
  $result = $query->execute();

  foreach ($result AS $row) {
    $data = unserialize($row->data);
    $ret['required'] = $data['required'];
    $ret['description'] = $data['description'];
    $ret['label'] = $data['label'];
    $ret['weight'] = $data['widget']['weight'];
    $ret['default'] = $data['default_value'];
    break;
  }

  if (empty($data['required'])) {
    $ret['values'] = swab_advice_group_add_none_to_values($ret['values'], '_none', t('- None -'));
  }

  return $ret;
}

function swab_advice_group_add_none_to_values($arr, $key, $val) {
  $arr = array_reverse($arr, TRUE);
  $arr[$key] = $val;
  return array_reverse($arr, TRUE);
}

function swab_advice_group_forms_get_empty_row() {
  $age = swab_advice_group_field_get_items('field_advc_age_ref', 'advice');
  $weight = swab_advice_group_field_get_items('field_advc_weight_ref', 'advice');

  return array(
    'adv_age' => array(
      '#type' => 'select',
      '#description' => check_plain($age['description']),
      '#options' => $age['values'],
      '#required' => $age['required'],
    ),
    'adv_weight' => array(
      '#type' => 'select',
      '#description' => check_plain($weight['description']),
      '#options' => $weight['values'],
      '#required' => $weight['required'],
    ),
    'adv_indication' => array(
      '#markup' => 'Indication placeholder',
    ),
    'adv_remarks' => array(
      '#markup' => 'Remarks placeholder',
    ),
    'adv_dosage' => array(
      '#markup' => 'Dosage placeholder',
    ),
    'adv_score' => array(
      '#markup' => 'Score placeholder',
    ),
    'weight' => array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#default_value' => 0,
      '#delta' => 10,
      '#title-display' => 'invisible',
    ),
  );
}

function swab_advice_group_add_advice_form_row() {
  $name = 'new_' . time();
  $row = swab_advice_group_forms_get_empty_row();

  $form['advices'] = array();
  $form['advices'][$name] = $row;
  $form['advices'][$name]['weight']['#attributes']['class'] = array('add-edit-advices-weight');

  $ret = '<tr class="draggable">';
  $ret .= '<td><div class="form-item form-type-select form-item-advices-' . str_replace('_', '-', $name) . '-adv-age"><select class="form-select" name="advices[' . $name . '][adv_age]" id="edit-advices-new-' . str_replace('_', '-', $name) . '-age">' . swab_advice_group_add_advice_form_row_options($form['advices'][$name]['adv_age']) . '</select></div></td>';
  $ret .= '<td><div class="form-item form-type-select form-item-advices-' . str_replace('_', '-', $name) . '-adv-weight"><select class="form-select" name="advices[' . $name . '][adv_weight]" id="edit-advices-new-' . str_replace('_', '-', $name) . '-weight">' . swab_advice_group_add_advice_form_row_options($form['advices'][$name]['adv_weight']) . '</select></div></td>';
  $ret .= '<td>Indication placeholder</td>';
  $ret .= '<td>Remarks placeholder</td>';
  $ret .= '<td>Dosage placeholder</td>';
  $ret .= '<td>Score placeholder</td>';
  $ret .= '<td><select name="advices[' . $name . '][weight]" id="edit-advices-' . str_replace('_', '-', $name) . '-weight" class="add-edit-advices-weight form-select"><option value="-10">-10</option><option value="-9">-9</option><option value="-8">-8</option><option value="-7">-7</option><option value="-6">-6</option><option value="-5">-5</option><option value="-4">-4</option><option value="-3">-3</option><option value="-2">-2</option><option value="-1">-1</option><option selected="selected" value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option><option value="10">10</option></select></td>';
  $ret .= '</tr>';
  $json = array('row' => $ret);
  print json_encode($json);
}

function swab_advice_group_add_advice_form_row_options($row) {
  $ret = '';
  foreach ($row['#options'] AS $key => $opt) {
    $ret .= '<option value="' . $key . '">' . $opt . '</option>';
  }
  return $ret;
}

function swab_advice_group_get_advice_form_item() {
  $args = arg();
  if (empty($args[3])) { // no post, show form
    switch ($args[2]) {
      case 'remarks':
        break;
    }
  }
  else { // do post

  }
  exit();
}
