<?php

/**
 * @file
 * The include file for the module
 *
 * This file holds some helper function used in the creation of the advice form.
 */

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

function swab_advice_group_get_vocabulary_by_name($vocabulary_name) {
  $vocabs = taxonomy_get_vocabularies(NULL);
  foreach ($vocabs as $vocab_object) {
    if ($vocab_object->machine_name == $vocabulary_name) {
      return $vocab_object;
    }
  }
  return NULL;
}

function swab_advice_group_get_advices_from_group($node) {
  $advices = array();
  if (!empty($node->field_advcg_advc['und'])) {
    foreach ($node->field_advcg_advc['und'] AS $key => $advice) {
      $advices[] = array(
        'nid' => $advice['nid'],
        'weight' => $key,
      );
    }
  }
  return $advices;
}

function swab_advice_group_get_advices_from_advice($advice) {
  $ret = array();
  $n = node_load($advice['nid']);
  if (!empty($n)) {
    $ret['age'] = '';
    if (!empty($n->field_advc_age_ref)) {
      $ret['age'] = $n->field_advc_age_ref['und'][0]['nid'];
    }
    $ret['weight'] = '';
    if (!empty($n->field_advc_weight_ref)) {
      $ret['weight'] = $n->field_advc_weight_ref['und'][0]['nid'];
    }
    $ret['ind'] = '';
    $ret['ind_tid'] = '';
    if (!empty($n->field_advc_indications)) {
      $terms = array();
      $tids = array();
      foreach ($n->field_advc_indications['und'] AS $indication) {
        $term = taxonomy_term_load($indication['tid']);
        $terms[] = $term->name;
        $tids[] = $indication['tid'];
      }
      $ret['ind'] = implode(', ', $terms);
      $ret['ind_tid'] = implode(',', $tids);
    }
    $ret['remarks'] = '';
    if (!empty($n->field_advc_remarks)) {
      $ret['remarks'] = truncate_utf8($n->field_advc_remarks['und'][0]['safe_value'], 100, TRUE, TRUE);
    }
    $ret['dosage'] = '-';
    if (!empty($n->field_advc_dsgs)) {
      $dosages = array();
      foreach ($n->field_advc_dsgs['und'] AS $dos) {
        $temp = node_load($dos['nid']);
        $dosages[] = $temp->title;
      }
      $ret['dosage'] = implode(', ', $dosages);
    }
    $ret['score'] = '';
  }
  $ret['row_weight'] = $advice['weight'];
  return $ret;
}

function swab_advice_group_reformat_results($values) {
  $results = array();
  foreach ($values as $key => $value) {
    $indications = array();
    $indication_explode = explode(',', $value['advice_group_' . $key]['advice_group_' . $key]['advice_group_' . $key . '_row']['adv_ind_table_' . $key . '_row']);
    if (!empty($indication_explode)) {
      foreach ($indication_explode AS $i) {
        $indications[] = array('tid' => $i);
      }
    }
    $results[] = array(
      'age' => $value['advice_group_' . $key]['advice_group_' . $key]['advice_group_' . $key . '_row']['adv_age_table_' . $key . '_row'],
      'weight' => $value['advice_group_' . $key]['advice_group_' . $key]['advice_group_' . $key . '_row']['adv_weight_table_' . $key . '_row'],
      'indication' => $indications,
      'nid' => $value['advice_group_' . $key]['advice_group_' . $key]['advice_group_' . $key . '_row']['adv_nid_table_' . $key . '_row'],
    );
  }
  return $results;
}

function swab_advice_group_update_field($n, $field, $lang, $fieldname = '', $value = '') {
  if ($value != '_none') {
    if (!empty($node->{$field})) {
      if (is_array($value)) {
        foreach ($value  AS $val) {
          $n->{$field}[$lang][] = $val;
        }
      }
      else {
        $n->{$field}[$lang][0][$fieldname] = $value;
      }
    }
    else {
      if (is_array($value)) {
        $n->{$field}[$lang] = array();
        foreach ($value  AS $val) {
          $n->{$field}[$lang][] = $val;
        }
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
  }
  return $n;
}

function swab_advice_group_include_references() {
  $reference_includes = references_dialog_attached();
  foreach ($reference_includes['js'] AS $js) {
    drupal_add_js($js);
  }
  foreach ($reference_includes['css'] AS $css) {
    drupal_add_css($css);
  }
}
