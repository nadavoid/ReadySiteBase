<?php
// $Id: linodef.admin.inc,v 1.1.2.2 2009/12/12 00:40:55 roidanton Exp $

/**
 * @file
 * Administrative interface for Linodef Filter.
 *
 * @ingroup linodef
 * @author Roi Danton
 */

/**
 * Form builder function for option settings.
 */
function linodef_options_form($form_state) {
	$options = new linodef_tags;

	$form['linodef_option_settings_active'] = array(
		'#type' => 'fieldset',
		'#title' => t('Permanently active'),
		'#description' => t('Checked options will be appended to every tag.'),
	);
	foreach ($options->valid_options as $option => $properties) {
		if ($properties['#active']) {
			$name_active = 'linodef_option_active_' . $option;
			$form['linodef_option_settings_active'][$name_active] = array(
				'#type' => 'checkbox',
				'#title' => $properties['#title'] . ' (' . $option . ')',
				'#default_value' => variable_get($name_active, FALSE),
			);
		}
	}

	$form['linodef_option_settings_auto'] = array(
		'#type' => 'fieldset',
		'#title' => t('Automatic option handling'),
	);
	foreach ($options->valid_options as $option => $properties) {
		if ($properties['auto']) {
			$name_auto = 'linodef_option_auto_' . $option;
			$form['linodef_option_settings_auto'][$name_auto] = array(
				'#type' => 'checkbox',
				'#title' => $properties['#title'] . ' (' . $option . ')',
				'#default_value' => variable_get($name_auto, TRUE),
				'#description' => $properties['auto']['#description'],
			);
		}
	}

	$form['linodef_option_settings_status'] = array(
		'#type' => 'fieldset',
		'#title' => t('Option status'),
		'#description' => t('Unchecked options are deactivated globally.'),
	);
	foreach ($options->valid_options as $option => $properties) {
		$name_status = 'linodef_option_status_' . $option;
		$form['linodef_option_settings_status'][$name_status] = array(
			'#type' => 'checkbox',
			'#title' => $properties['#title'] . ' (' . $option . ')',
			'#default_value' => variable_get($name_status, TRUE),
			'#description' => $properties['#description'],
		);
	}
	$form = system_settings_form($form);
	return $form;
}

/**
 * Form builder function for module settings.
 */
function linodef_settings_form($form_state) {
	$form['linodef_message'] = array(
		'#type' => 'fieldset',
		'#title' => t('Messages'),
	);
	$form['linodef_message']['linodef_message_accessdenied'] = array(
		'#type' => 'textfield',
		'#title' => t('Access denied message'),
		'#default_value' => variable_get('linodef_message_accessdenied', 'Access to !element denied.'),
		'#description' => theme('advanced_help_topic', 'linodef', 'filter_permissions') .' '. t('If the user has no access to an embedded element this message is shown instead of the embedded text.'),
		'#required' => TRUE,
	);
	if (module_exists('linodef_buttonsapi')) {
		$form['linodef_message']['linodef_message_accessdenied']['#description'] .= ' '. t('Also this message replaces items in the Linodef elementlist of an editorbutton. This is the case if the user has edit permissions but has no access to certain (or all) items of the list.');
	}
	$form['linodef_message']['linodef_message_accessdenied']['#description'] .= '<ul><li>'. t('!element contains the element type and the element id, e.g. Node 10 or Field field_example. It is recommended to use !element in this message.', array('!element' => '<strong>!element</strong>'));
	$form['linodef_message']['linodef_message_accessdenied']['#description'] .= '</li><li>'. t('If you have a multilanguage site you have to add the translation for your message manually.') .'</li></ul>';

	$form['linodef_message']['linodef_message_location']= array(
		'#type' => 'checkboxes',
		'#title' => t('Linodef message location'),
		'#default_value' => variable_get('linodef_message_location', array('inline' => 'inline', 'page' => 'page')),
		'#options' => array('inline' => t('Show messages inline'), 'page' => t('Show page messages')),
		'#description' => t('The place where the messages appear can be adjusted here: Page messages are displayed with the default drupal message system (messages in the green, yellow or red box).') .' ',
	);
	if (module_exists('linodef_buttonsapi')) {
		$form['linodef_message']['linodef_message_location']['#description'] .= t('Inline messages appear directly in the text and in the Linodef elementlist of an editorbutton.');
	}
	else {
		$form['linodef_message']['linodef_message_location']['#description'] .= t('Inline messages appear directly in the text.');
	}
	$form['linodef_message']['linodef_message_location']['#description'] .= '<br />'. t('Be careful when deactivating both as it will confuse your editorial staff members and your visitors in case some Linodef tags have a wrong syntax; what could happen since without messages the editor gets no cue when he enters a Linodef tag in a wrong manner. Not to mention that finding the fault might be an exhausting task without helpful hints.');

	$form['linodef_message']['linodef_message_level']= array(
		'#type' => 'radios',
		'#title' => t('Linodef message level'),
		'#default_value' => variable_get('linodef_message_level', 'all'),
		'#options' => array(
			'all' => t('Show all messages'),
			'warning' => t('Show all but status messages (hide deprecated notices)'),
			'access' => t('Show critical & access messages only'),
			'critical' => t('Show critical messages only')),
		'#description' => t('Normal (warning) messages give the user indicators about syntax faults, lacking (optional) modules to enable a feature he tries to use and the decision Linodef has made whereupon. Critical messages show up if the tag cannot be substituted due to lacking elements and access messages in case of denied access.'),
		'#required' => TRUE,
	);
	if (module_exists('linodef_buttonsapi')) {
		$form['linodef_message']['linodef_message_level']['#description'] .= ' '. t('In Linodef Buttons API critical messages appear if the entire button is empty because the entered element is not valid. If it is empty due to access restrictions for the current user then an access message is shown. Furthermore access messages include information about access restriction of a single button item.');
	}
	$form = system_settings_form($form);
	return $form;
}