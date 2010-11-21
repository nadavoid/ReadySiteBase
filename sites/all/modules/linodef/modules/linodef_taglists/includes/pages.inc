<?php
// $Id: pages.inc,v 1.1.2.2 2009/12/14 20:31:51 roidanton Exp $

/**
 * @file
 * Linodef forms and pages.
 *
 * @ingroup linodef_taglists
 * @author Roi Danton
 */

 /**
 * Menu callback: Create popup window.
 *
 * @param string $tlid
 *    The taglist id.
 * @param $type
 *      Type to set correct content. See hook_menu() function for its possible values.
 * @param $calling_module
 *      The name of the module that calls this function. Needed for javascript file and function to use when clicked on/submitting an item.
 * @return
 *      Nothing because the page shouldn't be included inside default content area. Instead print is used.
 *
 * @see linodef_taglists_menu(), linodef_taglists_theme()
 */
function linodef_taglists_popup($tlid, $type, $calling_module, $instanceID) {
	$title = t('Linodef taglist');
	$taglist = linodef_taglists_load($tlid);

	// Cache data for validation of autocomplete form. Change to use CTools cache.
	$cid = 'linodef_taglists:autocomplete:' . $tlid;
	// Cache the cid so other functions can retrieve it.
	cache_set('linodef_taglists:autocomplete:cid', $cid, 'cache', CACHE_TEMPORARY);
	cache_set($cid . ':taglist', $taglist->content, 'cache', CACHE_TEMPORARY);

	// Description.
	if ($taglist->properties['description_override']) {
		$text['description'] = $taglist->description;
	}
	else {
		// Source.
		$source = $taglist->source['source'];
		// Add view display and arguments.
		if ($taglist->source['display']) {
			$source .= ' ' . t('using display !display', array('!display' => $taglist->source['display']));
			$source .= $taglist->source['arguments'] ? ' ' . t('and arguments !arguments', array('!arguments' => $taglist->source['arguments'])) : '';
		}
		// Replace ID with the vocab name.
		if (module_exists('taxonomy') && ctype_digit($source)) {
			if ($vocabulary = taxonomy_vocabulary_load($source)) {
				$source = $vocabulary->name;
			}
		}
		// Add field labels.
		// view $content_type['type'] doesn't exist yet in default Drupal permissions.
		if ($taglist->sourcetype == 'field' && user_access('view content')) {
			$labels = linodef_taglists_field_labels($taglist->source['source']);
			if (is_array($labels)) {
				$source .= '(' . t('used in') . ' ' . implode(', ', $labels) . ')';
			}
		}
		$text['description'] = t('This window helps you to retrieve the Linodef tags for embedding %objecttype of the %source.', array('%objecttype' => $taglist->objecttype, '%source' => $taglist->sourcetype .' '. $source));
	}

	// Usage hints.
	switch ($type) {
		case "popup":
			$text['usage'] = t('Click on a list item in the list below to include the corresponding tag into your text.');
			$taglist_content = linodef_taglists_itemlist($taglist, $calling_module, $instanceID);
			break;
		case "autocomplete":
			$text['usage'] = t('Enter letters or words that are part of the text you are searching for into the textbox below. After some moments a list of suggestions appears. Choose the desired one so the corresponding tag shows up in the textbox.');
			$taglist_content = drupal_get_form('linodef_taglists_form_autocomplete', $taglist->tlid, $calling_module);
			break;
	}

	// Show tag options.
	if ($taglist->tagoptionstransformed) {
		$text['options'] = t('The tags include the following options:') . theme('item_list', $taglist->tagoptionstransformed['array']);
	}

	// Prevent devel module from spewing. Thx to advanced help.
	$GLOBALS['devel_shutdown'] = FALSE;
	// Suppress admin_menu. Thx to advanced help.
	module_invoke('admin_menu', 'suppress');
	// Don't use default theming header, instead use custom theme file from linodef. Therefore we use print().
	print theme('linodef_taglists_popup', $title, $taglist->name, $text, $taglist_content, $calling_module, $instanceID);
	return;
}

/**
 * Autocomplete page callback: Retrieve a pipe delimited string of autocomplete suggestions for existing elements.
 *
 * Parameters sent from #autocomplete_path at hook_menu(). The parameter after them is the inserted text by the user.
 *
 * @param string $tlid
 *    The taglist id.
 * @param string $string
 *    String the user entered in textfield.
 */
function linodef_taglists_autocomplete($tlid, $string = '') {
	$matches = array();
	$taglist = linodef_taglists_load($tlid);
	foreach ($taglist->content as $value) {
		if (!empty($string)) {
			if (strpos($value['desc'], $string) !== FALSE) {
				$matches[$value['tag']] = '<div class="reference-autocomplete">'. $value['desc'] .'</div>';
			}
		}
	}
	print drupal_to_js($matches);
	exit();
}

/**
 * Form builder for autocomplete textfield.
 *
 * @see linodef_taglists_popup()
 */
function linodef_taglists_form_autocomplete($form_state, $tlid) {
	$form['linodef_taglists_autocomplete'] = array(
		'#type' => 'textfield',
		'#title' => '',
		'#default_value' => '',
		'#autocomplete_path'  => 'linodef/autocomplete/' . $tlid,
		'#maxlength' => 512,
		/* maybe in a later version with Ajax '#ahah' => array(
			'wrapper' => $editor_textfield_id,
			'method' => 'append',
		),*/
	);
	$form['linodef_taglists_autocomplete_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	return $form;
}

/**
 * Validate the autocomplete form.
 */
function linodef_taglists_form_autocomplete_validate($form, &$form_state) {
	// Retrieve the cache id set on page function.
	$cid = cache_get('linodef_taglists:autocomplete:cid');

	if ($form_state['values']['linodef_taglists_autocomplete'] == '') {
		form_set_error('', t('Enter or select a tag that you want to include into your text.'));
	}
	elseif (($taglist_cache = cache_get($cid->data . ':taglist')) != 0) {
		$success = FALSE;
		foreach ($taglist_cache->data as $value) {
			// Allow tags with additional user input: comment or additional options (last one works only if comment not set).
			$value['tag'] = strtr($value['tag'], array(']' => ''));
			if (strpos($form_state['values']['linodef_taglists_autocomplete'], $value['tag']) !== FALSE && substr($form_state['values']['linodef_taglists_autocomplete'], -1) == ']') {
				$success = TRUE;
			}
		}
		if ($success === FALSE) {
			form_set_error('', t('Tag %tag is not valid or not part of this button. Either select another tag from the autocomplete list, use a different editor button or paste the tag you have entered manually into your text.', array('%tag' => $form_state['values']['linodef_taglists_autocomplete'])));
		}
	}
	else {
		form_set_error('', t('Empty cache, validation cancelled. Report this message to your site administrator.'));
	}
}

/**
 * Submit the autocomplete form.
 *
 * @see template_preprocess_linodef_taglists_popup()
 */
function linodef_taglists_form_autocomplete_submit($form, &$form_state) {
	drupal_set_message(t("The tag %tag has been submitted. In case it doesn't appear in your text don't worry but manually copy & paste the tag.", array('%tag' => $form_state['values']['linodef_taglists_autocomplete'])));
	
	// Caching the tag to submit it to the editors textarea.
	$cid = cache_get('linodef_taglists:autocomplete:cid');
	cache_set($cid->data .':submit', $form_state['values']['linodef_taglists_autocomplete'], 'cache', CACHE_TEMPORARY);
}
