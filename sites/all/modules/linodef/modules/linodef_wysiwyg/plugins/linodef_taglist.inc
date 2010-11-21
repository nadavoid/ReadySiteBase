<?php
// $Id: linodef_taglist.inc,v 1.1.2.3 2009/12/15 12:28:18 roidanton Exp $

/**
 * @file
 * Wysiwyg API integration.
 */

/**
 * Implementation of hook_wysiwyg_plugin().
 */
function linodef_wysiwyg_linodef_taglist_plugin() {
	$plugins = array();
	$taglists = linodef_taglists_load_definition();
	$path = drupal_get_path('module', 'linodef_wysiwyg') . '/plugins/linodef_taglist';
	// Popup Javascript.
	drupal_add_js(drupal_get_path('module', 'linodef_taglists') . '/linodef_taglists_functions.js');
	foreach ($taglists as $tlid => $taglist) {
		// Send taglist id to Javascript. Can't be done with Drupal.settings or own namespace
		// since those objects are loaded after the JS files which is too late.
		drupal_add_js('linodef_wysiwyg_add_taglist(' . $tlid . ')', 'inline');
		// Use popup for list widgets temporarly.
		$widget = ($taglist->properties['widget'] == 'list') ? 'popup' : $taglist->properties['widget'];
		if ($widget != 'list') {
			$settings = array(
				'dialog' => array(
					// Instance ID is appended to the path in Javascript.
					'url' => base_path() . 'linodef/taglist/' . $tlid . '/' . $widget . '/linodef_wysiwyg',
					'width' => 700,
					'height' => 500,
				),
			);
		}
		else {
			// Use wysiwyg JS to create drop down somehow.
			$settings = array();
		}
		$plugins['linodef_taglist_' . $tlid] = array(
			'title' => t('Taglist') . ' ' . $taglist->tlid . ': ' . $taglist->name,
			'vendor url' => 'http://drupal.org/project/linodef',
			// The path to the button's icon; defaults to
			// '/[path-to-module]/[plugins-directory]/[plugin-name]/images'.
			'icon path' => $path . '/images',
			// The button image filename; defaults to '[plugin-name].png'.
			'icon file' => 'linodef_taglist.gif',
			// The button title to display on hover.
			'icon title' => t('Linodef taglist') . ': ' . $taglist->name,
			// An alternative path to the integration JavaScript; defaults to
			// '[path-to-module]/[plugins-directory]/[plugin-name]'.
			'js path' => $path,
			// An alternative filename of the integration JavaScript; defaults to
			// '[plugin-name].js'.
			'js file' => 'linodef_taglist.js',
			// An array of settings for this button. Required, but API is still in flux.
			'settings' => $settings,
			// TinyMCE-specific: Additional HTML elements to allow in the markup.
			'extended_valid_elements' => array(
				'img[class|src|border=0|alt|title|width|height|align|name|style]',
				'table'
			),
		);
	}
	return $plugins;
}
