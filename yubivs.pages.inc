<?php
// $Id$
// vim: ts=2 sw=2 expandtab filetype=php:

/**
 * @file 
 * Pages for yubikey validation server implementation
 */


/**
 * Validate page
 */
function yubivs_node_yubikey_validate($node) {
  return drupal_get_form('yubivs_yubikey_validate_form', $node);
}


/**
 * Init page
 */
function yubivs_node_yubikey_init($node) {
  return drupal_get_form('yubivs_yubikey_init_form', $node);
}


function yubivs_yubikey_validate_form($form, &$form_state) {
  $node = reset($form_state['build_info']['args']);

  $form['otp'] = array(
    '#type' => 'textfield', 
    '#title' => t('Please OTP here'), 
    '#size' => 60, 
    '#maxlength' => 44, 
    '#default_value' => 'gggggggggggggggggggggggggggggggg',
    '#required' => TRUE,
  );

  $form['validate'] = array(
    '#type' => 'submit',
    '#value' => t('Validate'),
  );

  $form['#validate'] = array('yubivs_yubikey_otp_form_validate');

  return $form;
}


function yubivs_yubikey_init_form($form, &$form_state) {
  $node = reset($form_state['build_info']['args']);

  $form['otp'] = array(
    '#type' => 'textfield', 
    '#title' => t('Please OTP here'), 
    '#size' => 60, 
    '#maxlength' => 44,
    '#default_value' => 'gggggggggggggggggggggggggggggggg',
    '#required' => TRUE,
  );

  $form['initialize'] = array(
    '#type' => 'submit',
    '#value' => t('Initialize'),
  );

  $form['#validate'] = array('yubivs_yubikey_otp_form_validate');

  return $form;
}


function yubivs_yubikey_otp_form_validate($form, $form_state) {

  // Verify token length
  $len = strlen($form_state['values']['otp']);
  if ($len < 32) {
    form_set_error('yubikey_init', t("Should be at least @chars long", array('@chars' => 32)));
  }
  if ($len > 44) {
    form_set_error('yubikey_init', t("Shouldn't be more than @chars long", array('@chars' => 44)));
  }
  unset($len);

  // Verify content
  if (!preg_match('#^[cbdefghijklnrtuv]*$#', $form_state['values']['otp'])) {
    form_set_error('yubikey_init', t("This doesn't look like a modhex format"));
  }
}

/**
 * Config form
 */
function yubivs_config_form($form, &$form_state) {

  $field = field_info_field('yubivs_ksm');
  //dsm($field);

  $instance = array(
    'label' => t('Yubikey KSM Url'),
    'description' => '',
    'entity_type' => 'text',
    'widget' => array(
      'module' => 'text',
      'type' => 'text_textfield',
      'settings' => array(
        'size' => 25,
      ),
    ),
    'settings' => array(
      'text_processing' => FALSE,
    ),
    'bundle' => 'config',
    'required' => TRUE,
  );

  $form_state['field'] = array(
    'yubivs_ksm' => array(
      LANGUAGE_NONE => array(
        'field' => $field,
        'array_parents' => array('yubivs_ksm'),
      ),
    ),
  );

  $items = array();

  dsm($form_state);
  $field_item_count = 0;
  if (isset($form_state['complete form']) && isset($form_state['complete form']['yubivs_ksm'])) {
    foreach ($form_state['complete form']['yubivs_ksm'] as $name => $item) {
      if (!is_numeric($name) || 
          !is_array($item) || 
          !isset($item['#entity_type']) ||
          $item['#entity_type'] != 'text' ||
          !isset($item['value']) || 
          !is_array($item['value'])){
        continue;
      }
      $items[] = array('value' => $item['value']['#default_value']);
      $field_item_count ++;
    }
    array_pop($items);
    $field_item_count --;
    $items[] = array('value' => $form_state['input']['value']);
  }

  $form_state['field_item_count'] = $field_item_count;

  $field_elements = field_multiple_value_form($field, $instance, LANGUAGE_NONE, $items, $form, $form_state);
  
  dsm($field_elements);
  //dsm($form_state);

  $form['yubivs_ksm'] = $field_elements;

  //dsm($form);


  

  return system_settings_form($form);
}


// EOF
