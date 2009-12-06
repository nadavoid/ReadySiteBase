<?php
// $Id: linodef_buttonsapi.admin.inc,v 1.1.2.1 2008/11/30 13:00:31 roidanton Exp $

/**
 * @file
 * Admin pages for Linodef Buttons API.
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Helper function for linodef_buttonsapi_form_linodef_theme_settings_alter().
 *
 * Adds forms for module settings.
 */
function _linodef_buttonsapi_form_linodef_theme_settings_alter(&$form, $form_state) {
  // Shift system_settings_form buttons.
  $weight = isset($form['buttons']['#weight']) ? $form['buttons']['#weight'] : 0;
  $form['buttons']['#weight'] = $weight + 1;

  $form['linodef_buttonsapi'] = array(
    '#type' => 'fieldset',
    '#title' => t('Linodef buttons'),
  );

  $form['linodef_buttonsapi']['linodef_buttonsapi_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include default comments into Linodef tags'),
    '#default_value' => variable_get('linodef_buttonsapi_comments', 0),
    '#description' => theme('advanced_help_topic', 'linodef', 'filter_comments') .' '. t('If enabled, the buttons will include default comments into the Linodef tags. For nodes and fields the node title is used as comment, for taxonomy terms the term name is used.'),
  );
  $form['linodef_buttonsapi']['itemsort'] = array(
    '#type' => 'fieldset',
    '#title' => t('Item sort type & sequence for buttons'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['linodef_buttonsapi']['itemsort']['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Buttons with field content'),
  );
  $form['linodef_buttonsapi']['itemsort']['fields']['linodef_buttonsapi_itemsort_field_method'] = array(
    '#type' => 'select',
    '#title' => t('Sort by'),
    '#default_value' => variable_get('linodef_buttonsapi_itemsort_field_method', 'vid'),
    '#options' => array('vid' => t('field ID'), 'nid' => t('node ID')),
    '#description' => t('Select a method by which the field content should be sorted.'),
    '#required' => TRUE,
  );
  $form['linodef_buttonsapi']['itemsort']['fields']['linodef_buttonsapi_itemsort_field_sequence'] = array(
    '#type' => 'radios',
    '#title' => t('Sort sequence'),
    '#default_value' => variable_get('linodef_buttonsapi_itemsort_field_sequence', 'desc'),
    '#options' => array('asc' => t('Ascending'), 'desc' => t('Descending')),
    '#required' => TRUE,
  );
  $form['linodef_buttonsapi']['itemsort']['nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Buttons with node titles'),
  );
  $form['linodef_buttonsapi']['itemsort']['nodes']['linodef_buttonsapi_itemsort_node_method'] = array(
    '#type' => 'select',
    '#title' => t('Sort by'),
    '#default_value' => variable_get('linodef_buttonsapi_itemsort_node_method', 'title'),
    '#options' => array('title' => t('node title'), 'created' => t('node creation time'), 'changed' => t('time of the last node update')), // 'nid' => t('node ID'), 'uid' => t('node author'), 'language' => t('language'),
    '#description' => t('Select a method by which the node titles should be sorted.'),
    '#required' => TRUE,
  );
  $form['linodef_buttonsapi']['itemsort']['nodes']['linodef_buttonsapi_itemsort_node_sequence'] = array(
    '#type' => 'radios',
    '#title' => t('Sort sequence'),
    '#default_value' => variable_get('linodef_buttonsapi_itemsort_node_sequence', 'asc'),
    '#options' => array('asc' => t('Ascending'), 'desc' => t('Descending')),
    '#required' => TRUE,
  );
}