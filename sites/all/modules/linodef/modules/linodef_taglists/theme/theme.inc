<?php
// $Id: theme.inc,v 1.1.2.2 2009/12/14 20:31:51 roidanton Exp $

/**
 * @file
 * Preprocessors to fill variables for templates.
 *
 * @ingroup linodef_taglists
 * @author Roi Danton
 */

/**
 * Theme preprocess for popup.
 */
function template_preprocess_linodef_taglists_popup(&$variables) {
	// Add favicon.
	if (theme_get_setting('toggle_favicon')) {
		drupal_set_html_head('<link rel="shortcut icon" href="' . check_url(theme_get_setting('favicon')) . '" type="image/x-icon" />');
	}

	global $theme;

	// Load the javascript of the calling module for inserting the code into the editor.
	drupal_add_js(drupal_get_path('module', $variables['calling_module']) . '/' . $variables['calling_module'] . '_functions.js');

	// Autocomplete: Include tag into textfield on page reload after submitting.
	$cid_obj = cache_get('linodef_taglists:autocomplete:cid');
	$cid = $cid_obj->data . ':submit';
	if (($submitted_tag = cache_get($cid)) != 0) {
		drupal_add_js('$(function(){ ' . $variables['calling_module'] .'_onclick(\'' . $submitted_tag->data . '\',\'' . $variables['instanceID'] . '\'); });', 'inline');
		// Flush the submitted value.
		cache_clear_all($cid, 'cache');
	}

	// Focus the autocomplete textfield.
	drupal_add_js('$(function(){ $("#edit-linodef-taglists-autocomplete").focus(); });', 'inline');

	$variables['closebutton'] = '<a href="javascript:window.close()"><img src="' . base_path() . drupal_get_path('module', 'linodef_taglists') . '/theme/druplicon_linodef01_popupclose.png" title="' . t('Close window') . '" alt="' . t('Close window') . '" /></a>';
	$variables['head_title']        = variable_get('site_name', 'Drupal') . ' | ' . $variables['title'] . ': ' . $variables['taglist_name'];
	$variables['base_path']         = base_path();
	$variables['head']              = drupal_get_html_head();
	$variables['language']          = $GLOBALS['language'];
	$variables['language']->dir     = $GLOBALS['language']->direction ? 'rtl' : 'ltr';
	$variables['messages']          = theme('status_messages');

	// Additional CSS.
	drupal_add_css(drupal_get_path('module', 'linodef_taglists') . '/theme/linodef-taglists-popup.css');
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
