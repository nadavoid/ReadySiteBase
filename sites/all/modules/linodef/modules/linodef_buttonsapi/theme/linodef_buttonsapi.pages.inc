<?php
// $Id: linodef_buttonsapi.pages.inc,v 1.1.2.2 2008/11/30 13:00:31 roidanton Exp $

/**
 * @file
 * Page Handler for Linodef Buttons API.
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Menu callback: Create popup window.
 *
 * @param $type
 *      Type to set correct content. See hook_menu() function for its possible values.
 * @param $calling_module
 *      The name of the module that calls this function. Needed for javascript file and function to use when clicked on/submitting an item.
 * @param string $drupal_element
 *      The element to retrieve the data from. This has to be a supported element type. See Linodef advanced help.
 * @param string $option_string_path
 *      A string containing tag options, separated by "_" (passed from a path).
 * @return
 *      Nothing because the page shouldn't be included inside default content area. Instead print is used.
 *
 * @see linodef_buttonsapi_menu(), linodef_buttonsapi_theme(), linodef_buttons_get_itemlist()
 */
function _linodef_buttonsapi_popup($type, $calling_module, $drupal_element, $option_string_path) {
    $button_data_list = linodef_buttonsapi_itemlist($calling_module, $drupal_element, $option_string_path);

    // For validation function. Downside: This variable could be very very big and therefore is bad for db size. Furthermore if several users send the autocomplete message at the same time the validation could fail for some users. If there are other ways to pass the data to the validation function then submit your ideas/patch to the Linodef issue queue.
    variable_set('linodef_buttonsapi_buttondata', $button_data_list['data']);

    $explanation = t('This window helps you to retrieve the Linodef tags for embedding %elementtype of the %elementinput.', array('%elementtype' => $button_data_list['element type']['output'], '%elementinput' => $button_data_list['element type']['input'] .' '. $button_data_list['name'])) .' ';

    switch ($type) {
    case "popup":
        $content = $button_data_list['content'];
        $explanation .= t('Click on a list item in the list below to include the corresponding tag into your text.');
        break;
    case "popup-ac":
        $content = drupal_get_form('linodef_buttonsapi_autocomplete_form', $calling_module, $drupal_element, $option_string_path);
        $explanation .= t('Enter letters or words that are part of the text you are searching for into the textbox below. After some moments a list of suggestions appears. Choose the desired one so the corresponding tag shows up in the textbox. To submit the tag to your text press "Return" or copy & paste it by hand.');
        break;
    }
    
    // Display tag options.
    if (is_array($button_data_list['options'])) {
        $options = array_keys($button_data_list['options']);
        if (is_array($options)) {
            $explanation .= '<br />'. t('The tags include the following options:');
            $explanation .= theme('item_list', $options);
        }
    }

    // Prevent devel module from spewing. Thx to advanced help.
    $GLOBALS['devel_shutdown'] = FALSE;
    // Suppress admin_menu. Thx to advanced help.
    module_invoke('admin_menu', 'suppress');
    // Don't use default theming header, instead use custom theme file from linodef. Therefore we use print().
    print theme('linodef_buttonsapi_popup', $content, $explanation, $calling_module, $button_data_list['name']);
    return;
}

/**
 * Theme: Process page variables for Linodef popup template.
 */
function template_preprocess_linodef_buttonsapi_popup(&$variables) {
  // Add favicon.
  if (theme_get_setting('toggle_favicon')) {
    drupal_set_html_head('<link rel="shortcut icon" href="'. check_url(theme_get_setting('favicon')) .'" type="image/x-icon" />');
  }

  global $theme;

  // Load the javascript of the calling module for inserting the code into the editor.
  drupal_add_js(drupal_get_path('module', $variables['calling_module']) .'/'. $variables['calling_module'] .'_functions.js');

  // Autocomplete: Include tag into textfield on page reload after submitting.
  if ($submitted_tag = variable_get('linodef_buttonsapi_submitted_tag', False)) {
    drupal_add_js('$(function(){ '. $variables['calling_module'] .'_onclick(\''. $submitted_tag .'\'); });', 'inline');
    // Additional db query. :/
    variable_del('linodef_buttonsapi_submitted_tag');
  }

  // Focus the autocomplete button.
  drupal_add_js('$(function(){ $("#edit-linodef-buttonsapi-autocomplete").focus(); });', 'inline');

  $variables['closebutton'] = '<a href="javascript:window.close()"><img src="'. base_path() . drupal_get_path('module', 'linodef_buttonsapi') . '/theme/druplicon_linodef01_popupclose.png" title="'. t('Close window') .'" alt="'. t('Close window') .'" /></a>';
  $variables['head_title']        = variable_get('site_name', 'Drupal') .' | '. $variables['element_name'];
  $variables['base_path']         = base_path();
  $variables['head']              = drupal_get_html_head();
  $variables['language']          = $GLOBALS['language'];
  $variables['language']->dir     = $GLOBALS['language']->direction ? 'rtl' : 'ltr';
  $variables['messages']          = theme('status_messages');

  // Additional CSS.
  drupal_add_css(drupal_get_path('module', 'linodef_buttonsapi') . '/theme/linodef-buttonsapi-popup.css');
  $variables['css']               = drupal_add_css();
  $css = drupal_add_css();
   /*// Remove theme css for the case the theme uses bad CSS.
  foreach ($css as $media => $types) {
    if (isset($css[$media]['theme'])) {
      $css[$media]['theme'] = array();
    }
  }*/
  $variables['styles']            = drupal_get_css($css);
  $variables['scripts']           = drupal_get_js();

  // Closure should be filled last.
  $variables['closure']           = theme('closure');
}