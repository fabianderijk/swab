<?php

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
    if ($vocab_object->name == $vocabulary_name) {
      return $vocab_object;
    }
  }
  return NULL;
}