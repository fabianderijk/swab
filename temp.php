<?php

function swab_advice_group_get_advice() {
  $args = arg();
  if ($_SESSION[SWAB_ADVICE_GROUP_SESSION]) {
    $_SESSION[SWAB_ADVICE_GROUP_SESSION] = array();
  }
  $_SESSION[SWAB_ADVICE_GROUP_SESSION][$args[2]] = $args[2];

  $header = array(t('Patient Age'), t('Patient Weight'), t('Indications'), t('Remarks'), t('Dosage Schema'), t('Score'));
  $rows = array();

  foreach ($_SESSION[SWAB_ADVICE_GROUP_SESSION] AS $advice) {
    $n = node_load($advice);
    $age = '';
    if (!empty($n->field_advc_age_ref)) {
      $temp = node_load($n->field_advc_age_ref['und'][0]['nid']);
      $age = $temp->title;
    }
    $weight = '';
    if (!empty($n->field_advc_weight_ref)) {
      $temp = node_load($n->field_advc_weight_ref['und'][0]['nid']);
      $weight = $temp->title;
    }
    $ind = '';
    if (!empty($n->field_advc_indications)) {
      $terms = array();
      foreach ($n->field_advc_indications['und'] AS $indication) {
        $term = taxonomy_term_load($indication['tid']);
        $terms[] = $term->name;
      }
      $ind = implode(', ', $terms);
    }
    $remarks = '';
    if (!empty($n->field_advc_remarks)) {
      $remarks = truncate_utf8($n->field_advc_remarks['und'][0]['safe_value'], 100, TRUE, TRUE);
    }
    $dosage = '';
    if (!empty($n->field_advc_dsgs)) {
      $dosages = array();
      foreach ($n->field_advc_dsgs['und'] AS $dos) {
        $temp = node_load($dos['nid']);
        $dosages[] = $temp->title;
      }
      $dosage = implode(', ', $dosages);
    }
    $score = '';
    $rows[] = array($age, $weight, $ind, $remarks, $dosage, $score);
  }
  $table = theme('table', array('header' => $header, 'rows' => $rows));
  $ret = array('table' => $table);
  print json_encode($ret);
}
