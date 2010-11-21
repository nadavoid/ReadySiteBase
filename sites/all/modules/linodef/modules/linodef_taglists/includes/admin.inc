<?php
// $Id: admin.inc,v 1.1.2.2 2009/12/19 02:38:17 roidanton Exp $

/**
 * @file
 * Admin forms and pages.
 *
 * @ingroup linodef_taglists
 * @author Roi Danton
 */

/**
 * Form builder function for linodef_taglists_form_linodef_settings_form_alter().
 *
 * Adds additional settings to linodef settings form.
 */
function _linodef_taglists_form_linodef_settings_form_alter(&$form, $form_state) {
	// Shift system_settings_form buttons.
	$weight = isset($form['buttons']['#weight']) ? $form['buttons']['#weight'] : 0;
	$form['buttons']['#weight'] = $weight + 1;

	$form['taglist_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Linodef taglists'),
	);

	$form['taglist_settings']['linodef_taglists_use_cache'] = array(
		'#type' => 'checkbox',
		'#title' => t('Caching taglists'),
		'#default_value' => variable_get('linodef_taglists_use_cache', 0),
		'#description' => t('Taglists will be cached temporary. This increases performance when editing a node. However the taglist is not updated with recently added elements until the cache is flushed.'),
	);
}

/**
 * Menu callback and form builder: taglist administration.
 */
function linodef_taglists_admin_taglists($form_state) {
	// Altered version of node_admin_content() && node_admin_nodes() in node.admin.inc.
	// Confirmation 'dialog' for deletion operation.
	if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
		return linodef_taglists_multiple_delete_confirm($form_state, array_filter($form_state['values']['taglists']));
	}
	/*$form['options'] = array(
		'#type' => 'fieldset',
		'#title' => t('Update options'),
		'#prefix' => '<div class="container-inline">',
		'#suffix' => '</div>',
	);*/
	$form['options']['operation'] = array(
		'#type' => 'hidden',
		'#options' => array('delete' => t('Delete')),
		'#default_value' => 'delete',
	);
	$form['options']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete selected'),
		'#submit' => array('linodef_taglists_admin_taglists_submit'),
	);

	// katbailey: RoiDanton: since it's just come into Drupal core for D7 I've been looking into how it's supposed
	// to work and have posted some example code here: http://drupal.org/node/610068#comment-2194992, which I assume
	// will hold for ctools in d6, more or less.
  // katbailey: RoiDanton: oh, except there's now an ajax_deliver function which is a wrapper around ajax_render.
	// http://drupal.org/project/examples.
	// ctools_include('ajax');
	// ctools_add_js('ajax-responder');

	/*$header = array(
		array('data' => t('ID')),
		array('data' => t('Name')),
		array('data' => t('Source')),
		array('data' => t('Tagoptions')),
		array('data' => t('Operations')),
	);*/
	
	$taglists = linodef_taglists_load_definition();
	$taglists_as_formoption = array();

	if ($taglists) {
		foreach ($taglists as $taglist) {
			$taglists_as_formoption[$taglist->tlid] = '';
			$properties = $tagoptions = '';

			// Source.
			$source = $taglist->source['source'];
			// Add view display and arguments.
			if ($taglist->source['display']) {
				$source .= '<br /><span class="description">(' . $taglist->source['display'];
				$source .= $taglist->source['arguments'] ? ' | ' . $taglist->source['arguments'] : '';
				$source .= ')</span>';
			}
			// Add the vocab name.
			if (module_exists('taxonomy') && ctype_digit($source)) {
				if ($vocabulary = taxonomy_vocabulary_load($source)) {
					$source .= '<br /><span class="description">(' . $vocabulary->name . ')</span>';
				}
			}
			// Add field labels.
			if ($taglist->sourcetype == 'field') {
				$labels = linodef_taglists_field_labels($taglist->source['source']);
				if (is_array($labels)) {
					$source .= '<br /><span class="description">(' . t('used in') . ' ' . implode(', ', $labels) . ')</span>';
				}
				unset($labels);
			}

			// Options.
			if (is_array($taglist->tagoptions)) {
				$litter = linodef_tag_option_deprecated($taglist->tagoptions, t('Taglist %tlid: ', array('%tlid' => $taglist->tlid)));
				foreach ($taglist->tagoptions as $option => $op_value) {
					$tagoption = $option;
					if ($op_value) {
						$tagoption .= '="' . check_plain($op_value) . '"';
					}
					$tagoptions .= l($tagoption, 'admin/build/taglists/nojs/' . $taglist->tlid . '/edit/options', array('attributes' => array('class' => 'use-ajax', 'title' => t('Edit')), 'html' => TRUE, 'fragment' => $option));
					$tagoptions .= '<br />';
				}
			}

			// Properties.
			$widget_array = linodef_taglists_widgets();
			$properties_array['widget']['#title'] = t('Widget');
			$properties_array['widget']['#value'] = $widget_array[$taglist->properties['widget']]['#title'];
			$properties_array['comment']['#title'] = t('Comments');
			$properties_array['comment']['#value'] = $taglist->properties['comment'] ? t('yes') : t('no');
			$properties_array['sort']['#title'] = t('Sort');
			if ($taglist->properties['sort_sequence'] == 'as view' || $taglist->properties['sort_sequence'] == 'as vocabulary') {
				$properties_array['sort']['#value'] = $taglist->properties['sort_sequence'];
			}
			else {
				$sort_array = linodef_taglists_sortcriteria($taglist->objecttype);
				$properties_array['sort']['#value'] = t('!sequence by !method', array('!sequence' => $sort_array['sequence'][$taglist->properties['sort_sequence']], '!method' => $sort_array['method'][$taglist->properties['sort_method']]));
			}
			foreach ($properties_array as $value) {
				$properties .= $value['#title'] . ': ' . l($value['#value'], 'admin/build/taglists/nojs/' . $taglist->tlid . '/edit/options', array('attributes' => array('class' => 'use-ajax', 'title' => t('Edit')), 'html' => TRUE)) . '<br />';
			}

			// Taglist fields.
			$form['id'][$taglist->tlid] = array('#value' => check_plain($taglist->tlid));
			$form['name'][$taglist->tlid] = array('#value' =>
				l($taglist->name, 'admin/build/taglists/nojs/' . $taglist->tlid . '/edit/objecttype', array('attributes' => array('class' => 'use-ajax', 'title' => t('Edit whole taglist')))) . '<br /><span class="description">' .
				check_plain($taglist->description) . '</span>');
			$form['source'][$taglist->tlid] = array('#value' =>
				l($taglist->sourcetype, 'admin/build/taglists/nojs/' . $taglist->tlid . '/edit/sourcetype', array('attributes' => array('class' => 'use-ajax', 'title' => t('Edit')))) . ' ' . 
				l($source, 'admin/build/taglists/nojs/' . $taglist->tlid . '/edit/source', array('attributes' => array('class' => 'use-ajax', 'title' => t('Edit')), 'html' => TRUE))
			);
			$form['tagoptions'][$taglist->tlid] = array('#value' => $tagoptions);
			$form['properties'][$taglist->tlid] = array('#value' => $properties);
			/*$rows[] = array(
				array('data' => $value->tlid, 'class' => 'id'),
				array('data' => $value->name, 'class' => 'name'),
				array('data' => $value->sourcetype . ' ' . $value->source, 'class' => 'source'),
				array('data' => $data_tagoptions, 'class' => 'tagoptions'),
				array('data' => l('Edit', 'admin/build/taglists/' . $value->tlid . '/edit'), 'class' => 'operations'),
			);*/
		}
	}
	$form['taglists'] = array('#type' => 'checkboxes', '#options' => $taglists_as_formoption);

	$form['add'] = array(
		'#type' => 'fieldset',
		'#title' => t('New taglist'),
		'#prefix' => '<div class="container-inline">',
		'#suffix' => '</div>',
	);
	$form['add']['link'] = array(
		'#value' => l(t('Add new taglist'), 'admin/build/taglists/nojs/0/add/objecttype', array('attributes' => array('class' => 'use-ajax'))),
	);
	$form['#theme'] = 'linodef_taglists_admin_taglists';
	return $form;
}

/**
 * Theme taglists administration overview.
 *
 * @ingroup themeable
 */
function theme_linodef_taglists_admin_taglists($form) {
	// Altered version of theme_node_admin_nodes() in node.admin.inc.
	$output = drupal_render($form['add']);

	$has_lists = isset($form['id']) && is_array($form['id']);
	$select_header = $has_lists ? theme('table_select_header_cell') : '';
	$header = array(
		$select_header,
		array('data' => t('ID')),
		array('data' => t('Name')),
		array('data' => t('Source')),
		array('data' => t('Tagoptions')),
		array('data' => t('Properties')),
	);
	if ($has_lists) {
		foreach (element_children($form['id']) as $key) {
			$rows[] = array(
				array('data' => drupal_render($form['taglists'][$key]), 'class' => 'selectbox'),
				array('data' => drupal_render($form['id'][$key]), 'class' => 'id'),
				array('data' => drupal_render($form['name'][$key]), 'class' => 'name'),
				array('data' => drupal_render($form['source'][$key]), 'class' => 'source'),
				array('data' => drupal_render($form['tagoptions'][$key]), 'class' => 'tagoptions'),
				array('data' => drupal_render($form['properties'][$key]), 'class' => 'properties'),
			);
		}
	}
	else {
		$rows[] = array(array('data' => t('No taglists available.'), 'colspan' => '6'));
	}

	$output .= theme('table', $header, $rows);
	if ($has_lists) {
		$output .= drupal_render($form['options']);
	}
	$output .= drupal_render($form);
	return $output;
}

/**
 * Validate linodef_taglists_admin_taglists form submissions.
 */
function linodef_taglists_admin_taglists_validate($form, &$form_state) {
	$taglists = array_filter($form_state['values']['taglists']);
	if (count($taglists) == 0) {
		form_set_error('', t('No items selected.'));
	}
}

/**
 * Process linodef_taglists_admin_taglists form submissions.
 */
function linodef_taglists_admin_taglists_submit($form, &$form_state) {
	// We need to rebuild the form to go to a second step. For example, to
	// show the confirmation form for the deletion of taglists.
	$form_state['rebuild'] = TRUE;
}

function linodef_taglists_multiple_delete_confirm(&$form_state, $taglists) {
	$form['taglists'] = array('#prefix' => '<ul>', '#suffix' => '</ul>', '#tree' => TRUE);
	// array_filter returns only elements with TRUE values
	foreach ($taglists as $tlid => $value) {
		$props = linodef_taglists_load_definition($tlid);
		$form['taglists'][$tlid] = array(
			'#type' => 'hidden',
			'#value' => $tlid,
			'#prefix' => '<li>',
			'#suffix' => t('Taglist') . ' ' .  $tlid . ': ' . check_plain($props->name) ."</li>\n",
		);
	}
	$form['operation'] = array('#type' => 'hidden', '#value' => 'delete');
	$form['#submit'][] = 'linodef_taglists_multiple_delete_confirm_submit';
	return confirm_form(
		$form,
		t('Are you sure you want to delete these items?'),
		'admin/build/taglists',
		t('This action cannot be undone.'),
		t('Delete all'),
		t('Cancel')
	);
}

function linodef_taglists_multiple_delete_confirm_submit($form, &$form_state) {
	if ($form_state['values']['confirm']) {
		foreach ($form_state['values']['taglists'] as $tlid => $value) {
			linodef_taglists_delete($tlid);
		}
		drupal_set_message(t('The items have been deleted.'));
	}
	$form_state['redirect'] = 'admin/build/taglists';
	return;
}

/**
 * Menu callback: ctools wizard.
 *
 * @params string $js
 *   Required by CTools ajax/modal, to set if js can be used. 0 or TRUE.
 * @params string $tlid
 *   Taglist ID.
 * @params string $operation
 *   Either add or edit.
 * @params string $step
 *   Required by CTools wizard, must be an argument of the path in hook_menu.
 */
function linodef_taglists_wizard($js, $tlid, $operation, $step) {
	if (!$step) {
		ctools_include('ajax');
		return ctools_ajax_render_error();
	}

	$jspath = $js ? 'ajax' : 'nojs';
	$form_info = array(
		'id' => 'linodef_taglists_wizard',
		'path' => 'admin/build/taglists/' . $jspath . '/' . $tlid . '/' . $operation . '/%step',
		'show trail' => TRUE,
		'show back' => TRUE,
		'show cancel' => TRUE,
		'ajax' => $js,
		//'ajax render' => 'linodef_taglists_wizard_render',
		'cancel path' => 'admin/build/taglists',
		'finish text' => ($operation == 'edit') ? t('Update taglist') : t('Create new taglist'),
		'order' => array(
			'objecttype' => t('Object type'),
			'sourcetype' => t('Source type'),
			'source'     => t('Source object'),
			'options'    => t('Options &amp; properties'),
		),
		'forms' => array(
			'objecttype' => array(),
			'sourcetype' => array(),
			'source' => array(),
			'options' => array(),
		),
	);

	$form_state = array(
		'cache name' => $form_info['id'] . $operation . $tlid,
		// Set current type of operation for usage in callbacks.
		'taglist_handling' => array('operation' => $operation, 'tlid' => $tlid)
	);

	ctools_include('wizard');
	$output = ctools_wizard_multistep_form($form_info, $step, $form_state);
	return $output;
}

function linodef_taglists_wizard_render(&$form_state, $output) {
	// Todo. Only return changed forms.
}

/**
 * Callback generated when the wizard process is finished.
 */
function linodef_taglists_wizard_finish(&$form_state) {
	$taglist = &$form_state['taglist'];

	// Create a real object from the cache.
	$recordmessage = linodef_taglists_save($taglist);

	// Debug: print('<pre>');print_r($taglist);print('</pre>');

	if ($recordmessage === FALSE) {
		drupal_set_message('Taglist could not be saved.', 'error');
	}
	elseif ($recordmessage == SAVED_UPDATED) {
		drupal_set_message(t('Taglist %id: %name has been updated.', array('%id' => $taglist->tlid, '%name' => $taglist->name)));
	}
	else {
		drupal_set_message(t('New taglist %id: %name has been saved.', array('%id' => $taglist->tlid, '%name' => $taglist->name)));
	}

	// Clear the cache.
	linodef_taglists_clear_cache($form_state['cache name']);
	//$form_state['redirect'] = 'admin/build/taglists';
}

/**
 * Callback generated when the 'next' button is clicked.
 */
function linodef_taglists_wizard_next(&$form_state) {
	// Update the cache with changes.
	// Debug: print('Form state array: <pre>');print_r($form_state);print('</pre>');
	linodef_taglists_set_cache($form_state['cache name'], $form_state['taglist']);
}

/**
 * Callback generated when the 'cancel' button is clicked.
 */
function linodef_taglists_wizard_cancel(&$form_state) {
	// Clear cache.
	linodef_taglists_clear_cache($form_state['cache name']);
}

/**
 * Form builder function for wizard.
 * Those form function should contain a hook or using plugins (see page_manager)
 * so other modules can add elements.
 */
function linodef_taglists_wizard_objecttype_form(&$form, &$form_state) {

	// Load cached data for default values if user used back button.
	$form_state['taglist'] = linodef_taglists_get_cache($form_state['cache name']);

	// Load existing taglist if editing.
	if ($form_state['taglist_handling']['operation'] == 'edit' && !$form_state['taglist']->objecttype) {
		$form_state['taglist'] = linodef_taglists_load_definition($form_state['taglist_handling']['tlid']);
	}

	$form['type'] = array(
		'#type' => 'radios',
		'#title' => t('Select the object the tags should embed'),
		'#default_value' => $form_state['taglist']->objecttype ? $form_state['taglist']->objecttype : 'nodes',
		'#options' => linodef_taglists_objecttype()
	);
}

/**
 * Validation handler for objecttype.
 */
function linodef_taglists_wizard_objecttype_form_validate($form, &$form_state) {
	if (!$form_state['values']['type']) {
		form_set_error('', t('Choose an objecttype.') . ' ' . t('Selection also required on cancel/back due to ctools wizard restrictions.'));
	}
}

/**
 * Submit handler to prepare needed values for storing in cache.
 */
function linodef_taglists_wizard_objecttype_form_submit(&$form, &$form_state) {
	$form_state['taglist']->objecttype = $form_state['values']['type'];
}

/**
 * Form builder function for wizard.
 */
function linodef_taglists_wizard_sourcetype_form(&$form, &$form_state) {
	$form_state['taglist'] = linodef_taglists_get_cache($form_state['cache name']);

	// Load existing taglist if jumped directly to this step.
	if ($form_state['taglist_handling']['operation'] == 'edit' && !$form_state['taglist']->objecttype) {
		$form_state['taglist'] = linodef_taglists_load_definition($form_state['taglist_handling']['tlid']);
	}

	$form['history'] = array(
		'#value' => $form_state['taglist']->objecttype
	);

	$form['type'] = array(
		'#type' => 'radios',
		'#title' => t('Create the taglist by'),
		'#default_value' => $form_state['taglist']->sourcetype ? $form_state['taglist']->sourcetype : FALSE,
		'#options' => linodef_taglists_wizard_form_sourcetype_options($form_state['taglist']->objecttype),
	);
	// Debug: print('Form state array: <pre>');print_r($form_state);print('</pre>');
}

/**
 * Validation handler for sourcetype.
 */
function linodef_taglists_wizard_sourcetype_form_validate($form, &$form_state) {
	if (!$form_state['values']['type']) {
		form_set_error('', t('Choose a sourcetype.') . ' ' . t('Selection also required on cancel/back due to ctools wizard restrictions.'));
	}
}

/**
 * Submit handler to prepare needed values for storing in cache.
 */
function linodef_taglists_wizard_sourcetype_form_submit($form, &$form_state) {
	$form_state['taglist']->sourcetype = $form_state['values']['type'];
	if ($form_state['taglist']->sourcetype == 'getallviews') {
		// Jump to a certain step.
		$form_state['clicked_button']['#next'] = 'options';
		$form_state['taglist']->source['source'] = '';
	}
}

/**
 * Form builder function for wizard.
 */
function linodef_taglists_wizard_source_form(&$form, &$form_state) {
	$form_state['taglist'] = linodef_taglists_get_cache($form_state['cache name']);

	// Load existing taglist if jumped directly to this step.
	if ($form_state['taglist_handling']['operation'] == 'edit' && !$form_state['taglist']->sourcetype) {
		$form_state['taglist'] = linodef_taglists_load_definition($form_state['taglist_handling']['tlid']);
	}

	$form['history'] = array(
		'#value' => $form_state['taglist']->objecttype . ' » ' . $form_state['taglist']->sourcetype
	);

	// Alter form when getallviews is selected and this step is called (happens when going back from options).
	if ($form_state['taglist']->sourcetype == 'getallviews') {
		$form['message'] = array('#value' => t('No source required.'), '#prefix' => '<div>', '#suffix' => '</div>');
		$form['skip'] = array('#type' => 'hidden', '#value' => TRUE);
	}
	else {
		ctools_include('dependent');
		$options = linodef_taglists_wizard_form_source_options($form_state['taglist']->objecttype, $form_state['taglist']->sourcetype);
		$form['source'] = array(
			'#type' => 'radios',
			'#title' => t('Choose the object'),
			'#default_value' => $form_state['taglist']->source['source'] ? $form_state['taglist']->source['source'] : NULL,
			'#options' => $options['source'],
			'#tree' => TRUE
		);
		// If views then select display and set arguments.
		if ($form_state['taglist']->sourcetype == 'view') {
			foreach ($options['source'] as $viewname) {
				$form[$viewname] = array(
					'#type' => 'select',
					'#title' => t('Displays for view !viewname', array('!viewname' => $viewname)),
					'#default_value' => $form_state['taglist']->source['display'] ? $form_state['taglist']->source['display'] : 'default',
					'#options' => $options['display'][$viewname],
					'#process' => array('ctools_dependent_process'),
					'#dependency' => array('radio:source' => array($viewname)),
				);
			}
			$form['arguments'] = array(
				'#type' => 'textfield',
				'#title' => t('View arguments (optional)'),
				'#default_value' => $form_state['taglist']->source['arguments'] ? $form_state['taglist']->source['arguments'] : '',
				'#description' => t('Make sure the entered arguments are valid for the selected view. Separate several arguments by !separator.', array('!separator' => ',')),
			);
		}
	}
}

/**
 * Validation handler for source.
 */
function linodef_taglists_wizard_source_form_validate($form, &$form_state) {
	if (!$form_state['values']['source'] && !$form_state['values']['skip']) {
		form_set_error('', t('Choose a source object.') . ' ' . t('Selection also required on cancel/back due to ctools wizard restrictions.'));
	}
}

/**
 * Submit handler to prepare needed values for storing in cache.
 */
function linodef_taglists_wizard_source_form_submit($form, &$form_state) {
	$form_state['taglist']->source['source'] = $form_state['values']['source'];
	if ($form_state['taglist']->sourcetype == 'view') {
		$form_state['taglist']->source['display'] = $form_state['values'][$form_state['values']['source']];
		$form_state['taglist']->source['arguments'] = $form_state['values']['arguments'];
	}
}

/**
 * Form builder function for wizard.
 *
 * Adding a hint to option description in case the option is disabled.
 * Deprecated options are not heeded yet.
 */
function linodef_taglists_wizard_options_form(&$form, &$form_state) {

	ctools_include('dependent');
	$form_state['taglist'] = linodef_taglists_get_cache($form_state['cache name']);

	// Load existing taglist if jumped directly to this step.
	if ($form_state['taglist_handling']['operation'] == 'edit' && !$form_state['taglist']->source['source'] && !$form_state['taglist']->sourcetype) {
		$form_state['taglist'] = linodef_taglists_load_definition($form_state['taglist_handling']['tlid']);
	}

	$type = '';
	switch ($form_state['taglist']->objecttype) {
	case 'nodes':
	case 'fields':
		$type = 'node';
		break;
	case 'terms':
		$type = 'term';
		break;
	case 'views':
		$type = 'view';
		break;
	}

	$history = $form_state['taglist']->objecttype . ' » ' . $form_state['taglist']->sourcetype;
	if ($form_state['taglist']->source['source']) {
		$history .= ' » ' . $form_state['taglist']->source['source'];
		$description_source = $form_state['taglist']->source['source'];
		// Add the vocab name.
		if (module_exists('taxonomy') && ctype_digit($form_state['taglist']->source['source'])) {
			if ($vocabulary = taxonomy_vocabulary_load($form_state['taglist']->source['source'])) {
				$history .= ' (' . $vocabulary->name . ')';
				$description_source .= ' (' . $vocabulary->name . ')';
			}
		}
	}

	$form['history'] = array(
		'#value' => $history
	);

	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name'),
		'#default_value' => $form_state['taglist']->name ? $form_state['taglist']->name : $form_state['taglist']->objecttype,
		'#description' => t('Appears in title of theme elements like popups.'),
	);
	$form['description'] = array(
		'#type' => 'textarea',
		'#title' => t('Description'),
		'#default_value' => $form_state['taglist']->description ? $form_state['taglist']->description : t('!objecttype derived from !sourcetype !source', array('!objecttype' => $form_state['taglist']->objecttype, '!sourcetype' => $form_state['taglist']->sourcetype, '!source' => $description_source)),
		'#description' => t('HTML allowed.'),
		'#rows' => 2
	);

	$form['description_override'] = array(
		'#type' => 'checkbox',
		'#title' => t('Use description for theme elements'),
		'#default_value' => $form_state['taglist']->properties['description_override'] ? $form_state['taglist']->properties['description_override'] : 0,
		'#description' => t('This overrides the default description of the popups.'),
	);

	// Properties.
	$form['properties']  = array(
		'#type' => 'fieldset',
		'#title' => 'List properties'
	);

	// Widget.
	$widgets = linodef_taglists_widgets();
	linodef_make_fapi_option($widgets);
	$form['properties']['widget'] = array(
		'#type' => 'select',
		'#title' => t('Default widget'),
		'#default_value' => $form_state['taglist']->properties['widget'] ? $form_state['taglist']->properties['widget'] : $widgets[0],
		'#description' => t('The widget to use for displaying the taglist.'),
		'#options' => $widgets,
		'#prefix' => '<div class="container-inline">',
		'#suffix' => '</div>'
	);

	// Sort criteria.
	if ($form_state['taglist']->sourcetype == 'view' || $form_state['taglist']->objecttype == 'views') {
		$form['properties']['sort_sequence'] = array(
			'#type' => 'hidden',
			'#title' => t('Sort sequence'),
			'#value' => 'as view',
		);
		$form['properties']['sort_method'] = array(
			'#type' => 'hidden',
			'#title' => t('Sort sequence'),
			'#value' => 'as view',
		);
	}
	elseif ($form_state['taglist']->objecttype == 'terms') {
		$form['properties']['sort_sequence'] = array(
			'#type' => 'hidden',
			'#title' => t('Sort sequence'),
			'#value' => 'as vocabulary',
		);
		$form['properties']['sort_method'] = array(
			'#type' => 'hidden',
			'#title' => t('Sort sequence'),
			'#value' => 'as vocabulary',
		);
	}
	else {
		$sort_array = linodef_taglists_sortcriteria($form_state['taglist']->objecttype);
		$sort_method_options = $sort_array['method'];
		$sort_seq_options = $sort_array['sequence'];
		$sort_method_default = array_keys($sort_method_options);
		$sort_method_default = $sort_method_default[0];
		$sort_seq_default = array_keys($sort_seq_options);
		$sort_seq_default = $sort_seq_default[0];
		$form['properties']['sort_sequence'] = array(
			'#type' => 'radios',
			'#title' => t('Sort sequence'),
			'#default_value' => $form_state['taglist']->properties['sort_sequence'] ? $form_state['taglist']->properties['sort_sequence'] : $sort_seq_default,
			'#options' => $sort_seq_options,
			'#prefix' => '<div class="container-inline">',
		);
		$form['properties']['sort_method'] = array(
			'#type' => 'select',
			'#title' => t('by'),
			'#default_value' => $form_state['taglist']->properties['sort_method'] ? $form_state['taglist']->properties['sort_method'] : $sort_method_default,
			'#options' => $sort_method_options,
			'#description' => t('The method by which the taglist content should be sorted.'),
			'#suffix' => '</div>'
		);
	}

	// Comment.
	$disabled_comment = FALSE;
	switch ($type) {
	case 'node' :
		$comment_description = t('The node titles are used as comment.');
		break;
	case 'term' :
		$comment_description = t('The term names are used as comment.');
		break;
	case 'view' :
		$comment_description = t('Views do not support predefined comments as the viewname is already visible in the tag.');
		$disabled_comment = TRUE;
		break;
	}
	$form['properties']['comment'] = array(
		'#type' => 'checkbox',
		'#title' => t('Include comment into tags'),
		'#default_value' => $form_state['taglist']->properties['comment'] ? $form_state['taglist']->properties['comment'] : 0,
		'#description' => theme('advanced_help_topic', 'linodef', 'filter_comments') .' '. t('The taglist will include a predefined comment into the tags.') . ' ' . $comment_description,
		'#attributes' => array('class' => 'container-inline'),
		'#disabled' => $disabled_comment
	);

	// Options.
	$supported_tags = new linodef_tags;
	foreach ($supported_tags->valid_options as $key => $value) {
		if (array_key_exists($type, $value['#types'])) {
			// Skip option if type is specified more exactly (used for fields (type node)).
			if (!is_bool($value['#types'][$type]) && $value['#types'][$type] != $form_state['taglist']->objecttype) {
				continue;
			}
			// Skip option contentvalue if field has no multiple values.
			if ($form_state['taglist']->objecttype == 'fields' && $key == 'contentvalue' && $form_state['taglist']->sourcetype != 'view') {
				$field_array = content_fields($form_state['taglist']->source['source']);
				if (!$field_array['multiple']) {
					continue;
				}
			}

			// Prepare default properties for certain fields.
			if ($key == 'type') {
				$default_value_option = 1;
				$default_value_value = $type;
				$disabled = TRUE;
				$hardcoded_value = FALSE;
			}
			elseif ($form_state['taglist']->objecttype == 'fields' && $key == 'content') {
				$default_value_option = 1;
				$default_value_value = ($form_state['taglist']->sourcetype == 'view') ? t('{First field of the view.}') : $form_state['taglist']->source['source'];
				$disabled = TRUE;
				$hardcoded_value = TRUE;
			}
			else {
				// Apply cached data when editing.
				if (isset($form_state['taglist']->tagoptions) && array_key_exists($key, $form_state['taglist']->tagoptions)) {
					$default_value_option = 1;
					if ($value['value']) {
						$default_value_value = $form_state['taglist']->tagoptions[$key];
					}
				}
				else {
					$default_value_option = 0;
					$default_value_value = FALSE;
				}
				$hardcoded_value = FALSE;
				$disabled = FALSE;
			}

			// Add hint if option is disabled in linodef settings.
			$name_status = 'linodef_option_status_' . $option;
			$option_disabled = '';
			if (variable_get($name_status, TRUE) == FALSE) {
				$option_disabled = ' ' . t('This option is disabled in <a href="@option-settings">Linodef tagoption settings</a>!', array('@option-settings' => url('admin/build/tagoptions')));
			}
			
			$form['options'][$key]  = array(
				'#type' => 'fieldset',
				'#attributes' => array('id' => $key),
				'#tree' => TRUE,
			);
			$form['options'][$key]['active'] = array(
				'#type' => 'checkbox',
				'#title' => $value['#title'] . ' (' . $key . ')',
				'#default_value' => $default_value_option,
				'#description' => $value['#description'] . $option_disabled,
				'#disabled' => $disabled,
			);
			if ($value['value']) {
				if (isset($value['value']['#valid']) && (!$value['value']['#valid_dynamic'] || (is_array($value['value']['#valid_dynamic']) && !in_array($form_state['taglist']->objecttype, $value['value']['#valid_dynamic'])))) {
					$form['options'][$key]['value'] = array(
						'#type' => 'select',
						'#default_value' => $default_value_value,
						'#description' => $value['value']['#description'],
						'#options' => linodef_taglists_wizard_form_options_valid_values($type, $value['value']['#valid']),
						'#disabled' => $disabled,
						'#process' => array('ctools_dependent_process'),
						'#dependency' => array('edit-' . $key . '-active' => array(1)),
					);
				}
				else {
					$form['options'][$key]['value'] = array(
						'#type' => 'textfield',
						'#default_value' => $default_value_value,
						'#description' => $value['value']['#description'],
						'#disabled' => $disabled,
						'#process' => array('ctools_dependent_process'),
						'#dependency' => array('edit-' . $key . '-active' => array(1)),
					);
					// Workaround for not passing disabled textfield content.
					if ($hardcoded_value) {
						$form['options'][$key]['value']['#value'] = $default_value_value;
					}
				}
			}
		}
	}
}

/**
 * Validation handler.
 */
function linodef_taglists_wizard_options_form_validate($form, &$form_state) {
	$supported_tags = new linodef_tags;
	// Debug: print('Form state array: <pre>');print_r($form_state['values']);print('</pre>');
	foreach ($form_state['values'] as $option => $value) {
		if (!empty($value['active']) && isset($supported_tags->valid_options[$option]['value']) && empty($value['value'])) {
			form_set_error('', t('Enter a value or deactivate option %option.', array('%option' => $option)) . ' ' . t('Correction also required on cancel/back due to ctools wizard restrictions.'));
		}
	}
}

/**
 * Submit handler to prepare needed values for storing in cache.
 *
 * linodef_tag_validate() must not be called because then disabled options
 * will not be saved.
 */
function linodef_taglists_wizard_options_form_submit($form, &$form_state) {
	$form_state['taglist']->name = $form_state['values']['name'];
	$form_state['taglist']->description = $form_state['values']['description'];
	$form_state['taglist']->properties['widget'] = $form_state['values']['widget'];
	$form_state['taglist']->properties['sort_sequence'] = $form_state['values']['sort_sequence'];
	$form_state['taglist']->properties['sort_method'] = $form_state['values']['sort_method'];
	$form_state['taglist']->properties['comment'] = $form_state['values']['comment'];
	$form_state['taglist']->properties['description_override'] = $form_state['values']['description_override'];
	// Reset old tagoptions.
	$form_state['taglist']->tagoptions = array();
	foreach ($form_state['values'] as $option => $value) {
		// Save activated options only even if $value['value'] is set.
		if (is_array($value) && $value['active'] == 1) {
			$form_state['taglist']->tagoptions[$option] = $value['value'] ? $value['value'] : FALSE;
		}
	}
}

/**
 * Get fitting source types.
 * Returns an array to use as #options in radios or select boxes
 */
function linodef_taglists_wizard_form_sourcetype_options($type) {
	$output = array();
	switch ($type) {
	case 'nodes':
		$output['contenttype'] = t('using a contenttype');
		break;
	case 'fields':
		$output['field'] = t('using a field');
		break;
	case 'terms':
		$output['vocabulary'] = t('using a vocabulary');
		break;
	case 'views':
		return array('getallviews' => t('creating a list of all views'));
	}
	$output['view'] = t('using a view');
	return array_reverse($output, TRUE);
}

/**
 * Get fitting objects.
 * Returns an array to use as #options in radios or select boxes
 */
function linodef_taglists_wizard_form_source_options($objecttype, $sourcetype) {
	$output = array();
	switch ($sourcetype) {
	case 'view':
		switch ($objecttype) {
		case 'nodes':
		case 'fields':
			$base_table = 'node';
			break;
		case 'terms':
			$base_table = 'term_data';
			break;
		}
		$views = views_get_all_views();
		foreach ($views as $view) {
			if ($view->base_table == $base_table) {
				$output['source'][$view->name] = $view->name;
				foreach ($view->display as $display_id => $value) {
					$output['display'][$view->name][$display_id] = $value->display_title;
				}
			}
		}
		return $output;
	case 'contenttype':
		$output['source'] = node_get_types('names'); // In D7 use node_type_get_names().
		return $output;
	case 'field':
		$fields = content_fields();
		foreach ($fields as $field) {
			$output['source'][$field['field_name']] = $field['field_name'];
			// Add field labels.
			$labels = linodef_taglists_field_labels($field['field_name']);
			if (is_array($labels)) {
				$output['source'][$field['field_name']] .= ' <span class="description">(' . t('used in') . ' ' . implode(', ', $labels) . ')</span>';
			}
			unset($labels);
		}
		return $output;
	case 'vocabulary':
		$vocabs = taxonomy_get_vocabularies();
		foreach ($vocabs as $vocab) {
			$output['source'][$vocab->vid] = $vocab->name;
		}
		return $output;
	}
}

/**
 * Returns an array to use as #options in radios or select boxes
 */
function linodef_taglists_wizard_form_options_valid_values($objecttype, $valid) {
	$output = array();
	foreach ($valid as $value => $properties) {
		if (!$properties['#dynamic']) {
			if (!is_array($properties['#types'])) {
				$output[$value] = $properties['#description'] . ' (' . $value . ')';
			}
			else {
				if (array_key_exists($objecttype, $properties['#types'])) {
					$output[$value] = $properties['#description'] . ' (' . $value . ')';
				}
			}
		}
	}
	return $output;
}

/**
 * Saves an object to the linodef taglist table.
 */
function linodef_taglists_save(&$object) {
	if (!isset($object->tlid)) {
		// Save new.
		return drupal_write_record('linodef_taglists', $object);
	}
	elseif (!empty($object->tlid)) {
		// Update existing.
		return drupal_write_record('linodef_taglists', $object, 'tlid');
	}
	return FALSE;
}

/**
 * Deletes a record from the linodef taglist table.
 */
function linodef_taglists_delete($tlid) {
	$taglist = linodef_taglists_load_definition($tlid);
	db_query('DELETE FROM {linodef_taglists} WHERE tlid = %d', $taglist->tlid);
	watchdog('content', 'Taglist: deleted %tlid: %title.', array('%tlid' => $taglist->tlid, '%title' => $taglist->name));
	drupal_set_message(t('Taglist %tlid: %title has been deleted.', array('%tlid' => $taglist->tlid, '%title' => $taglist->name)));
}

/**
 * Get the cached changes to a given task handler.
 */
function linodef_taglists_get_cache($name) {
	ctools_include('object-cache');
	$cache = ctools_object_cache_get('linodef_taglists', $name);
	/*if (!$cache) {
		$cache = linodef_taglists_load($name); // Load the list object to be locked.
		$cache->locked = ctools_object_cache_test('linodef_taglists', $name);
	}*/
	return $cache;
}

/**
 * Store changes to a task handler in the object cache.
 */
function linodef_taglists_set_cache($name, $obj) {
	ctools_include('object-cache');
	$cache = ctools_object_cache_set('linodef_taglists', $name, $obj);
}

/**
 * Remove an item from the object cache.
 */
function linodef_taglists_clear_cache($name) {
	ctools_include('object-cache');
	ctools_object_cache_clear('linodef_taglists', $name);
}
