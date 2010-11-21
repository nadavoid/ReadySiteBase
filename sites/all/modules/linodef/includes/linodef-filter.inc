<?php
// $Id: linodef-filter.inc,v 1.1.2.14 2009/12/15 23:37:57 roidanton Exp $

/**
 * @file
 *
 * The replace functions executed in the process filter step.
 *
 * @ingroup linodef
 * @author Roi Danton
 */

/**
 * Private function to return body with replaced tags.
 *
 * Search and replace the tags with internal links and node title or field values.
 *
 * @param $body
 *   Text on that the preg_replace function will be executed.
 */
function _linodef_filter_process(&$body, $format = -1) {
	// Debug: print('_linodef_filter_process has been called with format ' . $format . '<br />');
	// Tags with options, possible comment will be substituted.
	// Using preg_replace_callback() to avoid modificator 'e' (which escapes ',",\ and NULL).
	// Including backwards compatibility for RC2 option-value syntax without "": (?:=[a-zA-Z]*)*.
	$body = preg_replace_callback('@\[#([0-9a-zA-Z_]+),((?:[,0-9a-zA-Z]*(?:="[^"\]\r\n]*")*(?:=[a-zA-Z_]*)*)*)[^\]\r\n]*\]@', create_function('$treffer', 'return _linodef_filter_processtag($treffer[1], ' . $format . ', "$treffer[2]");'), $body);
	$body = preg_replace_callback('@\[#([0-9a-zA-Z_]+):([0-9a-z_]+),((?:[,0-9a-zA-Z]*(?:="[^"\]\r\n]*")*(?:=[a-zA-Z_]*)*)*)[^\]\r\n]*\]@', create_function('$treffer', 'return _linodef_filter_processtag($treffer[1], ' . $format . ', "$treffer[3]", $treffer[2]);'), $body);
	$body = preg_replace_callback('@\[#([0-9a-zA-Z_]+):([0-9a-z_]+):([0-9al]+),((?:[,0-9a-zA-Z]*(?:="[^"\]\r\n]*")*(?:=[a-zA-Z_]*)*)*)[^\]\r\n]*\]@', create_function('$treffer', 'return _linodef_filter_processtag($treffer[1], ' . $format . ', "$treffer[4]", $treffer[2], $treffer[3]);'), $body);

	// Tags without options.
	$preg = array(

		// Backwards compatibility for RC1/2 syntax.
		// Link Node ID with user defined text.
		"@\xf4\x80\x81\x8c#([0-9]+)\xf4\x80\x8f\x8d(.*?)\xf4\x80\x81\x8c/#\xf4\x80\x8f\x8d@se"                => "_linodef_find_node($1,'$2');",
		// Embed title or field values, add no link.
		"@\[#nl([0-9]+)\]@e"                                    => "_linodef_filter_processtag($1, $format, 'nolink');",
		"@\[#nl([0-9]+):([0-9a-z_]+)\]@e"                       => "_linodef_filter_processtag($1, $format, 'nolink', $2);",
		"@\[#nl([0-9]+):([0-9a-z_]+):([0-9]+)\]@e"              => "_linodef_filter_processtag($1, $format, 'nolink', $2, $3);",
		// Link term ID and embed term name.
		"@\[#t([0-9]+)\]@e"                                     => "_linodef_find_term($1, $format);",

		// Node/fields, alphanumeric for viewname.
		"@\[#([0-9]+)\]@e"                                           => "_linodef_find_nodesnfields($1, $format);",
		"@\[#([0-9a-zA-Z_]+?)\]@e"                                   => "_linodef_find_view($1, $format);",
		"@\[#([0-9]+) [^\]\r\n]+\]@e"                                => "_linodef_find_nodesnfields($1, $format);",
		"@\[#([0-9a-zA-Z_]+) [^\]\r\n]+\]@e"                         => "_linodef_find_view($1, $format);",
		"@\[#([0-9a-zA-Z_]+):([0-9a-z_]+)\]@e"                       => "_linodef_filter_processtag($1, $format, '', $2, 0, FALSE);",
		"@\[#([0-9a-zA-Z_]+):([0-9a-z_]+) [^\]\r\n]+\]@e"            => "_linodef_filter_processtag($1, $format, '', $2, 0, FALSE);",
		"@\[#([0-9a-zA-Z_]+):([0-9a-z_]+):([0-9al]+)\]@e"            => "_linodef_filter_processtag($1, $format, '', $2, $3, FALSE);",
		"@\[#([0-9a-zA-Z_]+):([0-9a-z_]+):([0-9al]+) [^\]\r\n]+\]@e" => "_linodef_filter_processtag($1, $format, '', $2, $3, FALSE);",

		// Inline comment which would not appear for readers, only editors.
		'@\[# [^\]\r\n]+]@' => '',

	);
	$body = preg_replace(array_keys($preg), array_values($preg), $body);

	return $body;
}

/**
 * Private function for _linodef_filter_process.
 * Processes tags and launches the function to retrieve the data.
 *
 * @param $id
 *   ID of the tag.
 * @param string $option_string
 *   Options as string.
 * @param string $obj_content
 *   Specific content of a object, e.g. field_name, display_name, title.
 * @param $content_value
 *   Number or string to get the desired value of multiple value fields or 0 if not set.
 * @param bool $optionsflag
 *   Set if options can be expected or not.
 * @return
 *   Returns a string containing the embedded content and/or messages.
 */
function _linodef_filter_processtag($id, $format = -1, $option_string = '', $obj_content = FALSE, $content_value = 0, $optionsflag = TRUE) {
	$message = '';
	if ($optionsflag && empty($option_string)) {
		// An empty option string shouldn't be called with a true optionsflag unless there is a needless comma left.
		$message .= linodef_message(array('custom' => t('Enter an option for Linodef tag with id %id or remove the comma inside the tag', array('%id' => $id))), 'warning');
		$options = array();
		$type = 'node';
	}
	else {
		// Evaluate type based on id layout. Could change in future if more object types uses alphanumerics as identifier.
		$type = (preg_match("/[a-zA-Z]/", (string) $id) != 0) ? 'view' : FALSE;
		// Transform id:value:value layout into corresponding options.
		if ($obj_content) {
			if ($optionsflag) {
				$option_string .= (substr($option_string, -1) == ',') ? '' : ',';
			}
			// An dynamic validation using the linodef_tags object would be better.
			// Therefore dynamic validation in linodef_tag_validate() has to be outsourced.
			if ($type == 'view' && $obj_content != 'title') {
				$option_string .= 'display="' . $obj_content . '"';
			}
			else {
				$option_string .= 'content="' . $obj_content . '"';
				$option_string .= $content_value ? ',' . 'contentvalue="' . (string) $content_value . '"' : '';
			}
		}
		// Would hurt differencation of user set type and evaluated type.
		// $option_string .= ($type == 'view') ? 'type="view"' : '';
		$validated_options = _linodef_filter_options_typevalidation($type, $message, $id, $option_string, $obj_content, $content_value);
	}

	// Hide tag if he has set different build mode.
	if (isset($validated_options['visibility']) && $validated_options['visibility'] != constant('LINODEF_BUILDMODE')) {
		return $message;
	}
	switch ($type) {
		case 'node':
			return _linodef_find_nodesnfields($id, $format, $validated_options) . $message;
			break;
		case 'term':
			return _linodef_find_term($id, $format, $validated_options) . $message;
			break;
		case 'view':
			return _linodef_find_view($id, $format, $validated_options) . $message;
			break;
	}
}

/**
 * Private function for _linodef_filter_process.
 * Validates compatibility between type and options. If no type is given evaluate it.
 *
 * @param &$type
 *   Object type.
 * @param &$message
 *   Message.
 * @param $id
 *   ID of the tag.
 * @param string $option_string
 *   Options as string.
 * @param string $obj_content
 *   Specific content of a object, e.g. field_name, display_name, title.
 * @param $content_value
 *   Number or string to get the desired value of multiple value fields or 0 if not set.
 * @return
 *   Returns a string containing the embedded content and/or messages.
 */
function _linodef_filter_options_typevalidation(&$type, &$message, $id, $option_string, $obj_content, $content_value) {
	$options = linodef_tag_validate($option_string);
	if (is_array($options)) {
		// 1. Get object type if given.
		if ($options['options']['type']) {
			$type = $options['options']['type']['#value'];
		}
		// 2. Validate options against object type. If type not set, determine best.
		if ($options['options']) {
			foreach ($options['options'] as $option => $value) {
				$properties = $options['valid_options'][$option];
				// Debug: print('(' . __FUNCTION__ . ') ');print('ID: <b>' . $id . '</b> Option: <b>' . $option . '</b> Properties array: <pre>');print('');print_r($properties);print('</pre><br />');
				// a. Check if option is compatible with object type.
				if ($type) {
					if (array_key_exists($type, $properties['#types'])) {
						if ($properties['value']['#valid_dynamic'] && !array_key_exists($type, $properties['value']['#valid'][$value['#validation_value']]['#types'])) {
							// Output message depending on the way the user set the value ('option=value' or :value).
							$msgoption = $obj_content ? (':' . $obj_content . '(' . t('Option: ') . $option . ')') : $option;
							$message .= linodef_message(array('warning' => array('option ignored' => array('%id' => $id, '%option' => $msgoption)), 'explanation' => array('value not compatible with objtype' => array('%objtype' => $type, '%value' => $value['#value']))));
						}
						else {
							// Set option.
							$validated_options[$option] = $value['#value'];
						}
					}
					else {
						$message .=  linodef_message(array('warning' => array('option ignored' => array('%id' => $id, '%option' => $option)), 'explanation' => array('option not compatible with objtype' => array('%objtype' => $type, '%option' => $option))));
					}
				}
				// b. Try to evaluate the best fitting type. First fit determines object type. Type evaluation based on ID layout already done above.
				else {
					// Evaluate type from a value.
					if ($properties['value']['#valid'][$value['#validation_value']]['#types']) {
						// Heed option value dependency on type.
						$type = key($properties['value']['#valid'][$value['#validation_value']]['#types']);
					}
					// Evaluate type from an option.
					elseif (!array_key_exists('node', $properties['#types'])) {
						reset($properties['#types']);
						$type = key($properties['#types']);
					}
					// Set option.
					$validated_options[$option] = $value['#value'];
				}
			}
		}
		// Create messages for empty, wrong and unknown options.
		if ($msg = $options['message']['value']['empty']) {
			foreach ($msg as $option => $value) {
				$message .= linodef_message(array('warning' => array('option ignored' => array('%id' => $id, '%option' => $option)), 'explanation' => array('empty value' => TRUE)));
			}
		}
		if ($msg = $options['message']['value']['wrong']) {
			foreach ($msg as $option => $value) {
				$key = key($options['valid_options'][$option]['value']['#valid']);
				if ($options['valid_options'][$option]['value']['#valid'][$key]['#types']) {
					$valid_values_ar = array();
					foreach ($options['valid_options'][$option]['value']['#valid'] as $vvalue => $properties) {
						if (array_key_exists($type, $properties['#types'])) {
							$valid_values_ar[] = $vvalue;
						}
					}
					$valid_values = implode(', ', $valid_values_ar);
				}
				else {
					$valid_values = $value['#valid'];
				}
				$message .= linodef_message(
					array(
						'warning' => array('option ignored' => array('%id' => $id, '%option' => $option)),
						'explanation' => array('wrong value' =>  array('%value' => $value['#value'], '%valid_values' => $valid_values)),
					)
				);
			}
		}
		if ($msg = $options['message']['unknown']) {
			foreach ($msg as $key => $value) {
				if ($key === '') {
					$message .= linodef_message(array('custom' => t('Enter a further option for Linodef tag with id %id or remove the last comma inside the tag', array('%id' => $id))), 'warning');
				}
				else {
					$message .= linodef_message(array('warning' => array('option unknown' => array('%id' => $id, '%option' => $key))));
				}
			}
		}
	}
	// Debug: print('(' . __FUNCTION__ . ') ');print('ID: <b>' . $id . '</b> Type: <b>' . $type . '</b> Option string: <b>' . $option_string . '</b> Option array: <pre>');print_r($options['options']);print('</pre> Validated option array: <pre>');print_r($validated_options);print('</pre> Message array: <pre>');print_r($options['message']);print('</pre><br />');
	// If no type is set (e.g. none or global tag options like nolink are used), set type to nid.
	if (!$type) {
		$type = 'node';
	}
	return $validated_options;
}

/**
 * Private function for _linodef_filter_process.
 *
 * @param string $nid
 *   Node ID of the node that will be linked.
 * @param array $options
 *   key is option name, value is option value
 * @return
 *   Returns a string containing the (linked) node title/field value or a hint if node/field/field-content not found.
 */
function _linodef_find_nodesnfields($nid, $format = -1, $options = array()) {
	if ($nid && $node = node_load($nid)) {
		// If translation option is set, find translated node with desired language. If no node in that language is available continue as usual.
		if (isset($options['translation']) && $language = $options['translation']) {
			if (module_exists('translation')) {
				/*if ($language == '*') {
					// Use language of current node without having to define it manually in the tag: http://drupal.org/node/650402
					//$embedding_node = How to retrieve?
					//$language = $embedding_node->translation->language;
				}*/
				if ($tnid = $node->tnid) {
					if ($translations = translation_node_get_translations($tnid)) {
						foreach ($translations as $value) {
							if ($value->language == $language) {
								$node = node_load($value->nid);
							}
						}
					}
				}
			}
			else {
				return linodef_message(
					array(
						'warning' => array('activate module' => 'content translation'),
						'explanation' => array('option to use in tag id' => array('%option' => 'translation', '%id' => $nid))
					)
				);
			}
		}
		if (node_access('view', $node)) {
			// Nodes.
			if (isset($options['override'])) {
				$output = $options['override'];
			}
			elseif (!$options['content']) {
				$output = $node->title;
			}
			elseif (substr($options['content'], 0, 6) != 'field_') {
				$node_array = get_object_vars($node);
				if ($options['content'] == 'content') {
					// Remove existing Linodef tags before doing markup to avoid self and cross referencing (causing an infinite recursion) until unlimited depths.
					foreach ($node_array as $key => $values) {
						if (isset($values[0]['value'])) {
							foreach ($values as $number => $value) {
								if ($value['value']) {
									$node_array[$key][$number]['value'] = linodef_removetags($value['value']);
								}
							}
						}
					}
					$node_array['body'] = linodef_removetags($node->body);
					$node_array['teaser'] = linodef_removetags($node->teaser);
					$sanitized_node = (object) $node_array;
					$sanitized_node = node_build_content($sanitized_node);
					$output = drupal_render($sanitized_node->content);
				}
				elseif ($options['content'] == 'body_buildmode') {
					$mode = (constant('LINODEF_BUILDMODE') == 'full') ? 'body' : 'teaser';
					$output = linodef_removetags($node_array[$mode]);
				}
				elseif ($options['content'] == 'body' || $options['content'] == 'teaser') {
					$output = linodef_removetags($node_array[$options['content']]);
				}
			}
			// Fields.
			elseif (property_exists($node, $options['content'])) {
				$fieldname = $options['content'];
				// Heed content permissions settings if module is enabled.
				if (module_exists('content_permissions')) {
					$content_perm = user_access('view '. $fieldname) ? TRUE : FALSE;
					if (!$content_perm) {
						return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'access denied')));
					}
				}
				$node_array = get_object_vars($node);
				// Get field information.
				// Content_fields() doesn't provide display settings added by other modules.
				$ctype = content_types($node->type);
				$field = $ctype['fields'][$fieldname];

				// Set formatter.
				$formatter = $field['display_settings'][constant('LINODEF_BUILDMODE')]['format'];
				if (isset($options['formatter'])) {
					// Validate the value of $options['formatter'] by getting all valid formatters for the current field type.
					$field_types = _content_field_types();
					$valid_formatters = $field_types[$field['type']]['formatters'];
					if (array_key_exists($options['formatter'], $valid_formatters)) {
						$formatter = $options['formatter'];
					}
					else {
						$message .= linodef_message(
							array(
								'warning' => array('option ignored' => array('%id' => $nid, '%option' => 'formatter')),
								'explanation' => array('wrong value' =>  array('%value' => $value, '%valid_values' => implode(',', array_keys($valid_formatters)))),
								'custom' => t('so fallback to default formatter %formatter', array('%formatter' => $formatter))
							)
						);
					}
				}
				// The field item(s) to be formatted (such as $node->field_foo[0] or $node->field_foo if the formatter handles multiple values itself).
				if ($options['contentvalue'] == 'all' && $valid_formatters[$formatter]['multiple values']) {
					$field_item = $node_array[$fieldname];
				}
				else {
					if ($options['contentvalue'] == 'all') {
						$message .= linodef_message(
							array(
								'warning' => array('option ignored' => array('%id' => $nid, '%option' => 'contentvalue')),
								'explanation' => array('wrong value' =>  array('%value' => $value, '%valid_values' => '0')),
								'custom' => t('so fallback to first value')
							)
						);
						$options['contentvalue'] = '0';
					}
					$options['contentvalue'] = (int) $options['contentvalue'];
					$field_item = $node_array[$fieldname][$options['contentvalue']];
				}
				// Remove self referencing tags: $output = str_replace('[#'. $nid .':'. $fieldname .']', '', $output);
				// This is possible until certain depth, but cross ref could still occur on deeper depth. Therefore recursion removed completely.
				// Remove existing Linodef tags before doing markup to avoid self and cross referencing (causing an infinite recursion and thus white page) until unlimited depths.
				$field_item = linodef_removetags($field_item);
				$output = content_format($field, $field_item, $formatter, $node) . $message;
				// Check if field values are set, otherwise output message.
				if (drupal_strlen($output) < 1) {
					$erroneous_tag[0] = '#' . $nid . ':' . $fieldname;
					// Set correct tag layout for error message heeding the translation option.
					if ($options['translation'] && $nid != $node->nid) {
						$erroneous_tag[2] = ',translation=' . $options['translation'];
					}
					// Check cardinality.
					if ($options['contentvalue'] == 0) {
						$erroneous_tag_str = ksort($erroneous_tag) ? implode($erroneous_tag) : '';
						return linodef_message(
							array(
								'custom' => t('Field %field of node %nid has no content and hence cannot be embedded. Create content for that field or revise the Linodef tag which contains %erroneous_tag', array('%field' => $fieldname, '%nid' => $node->nid, '%erroneous_tag' => $erroneous_tag_str))
							),
							'error'
						);
					}
					else {
						$erroneous_tag[1] = ':' . $options['contentvalue'];
						$erroneous_tag_str = ksort($erroneous_tag) ? implode($erroneous_tag) : '';
						return linodef_message(
							array(
								'custom' => t('Value %content_value of field %field of node %nid not found or has no content and hence cannot be embedded. Create content for that field or revise the Linodef tag which contains %erroneous_tag', array('%content_value' => $options['contentvalue'], '%field' => $fieldname, '%nid' => $node->nid, '%erroneous_tag' => $erroneous_tag_str))
							),
							'error'
						);
					}
				}
			}
			else {
				return linodef_message(
					array(
						'error' => array('field' => $options['content'], 'type' => 'not found'),
						'custom' => t('or is not assigned to content type %ctype', array('%ctype' => $node->type))
					)
				);
			}
			// Set nolink automatically for content with HTML block level elements.
			if (variable_get('linodef_option_auto_nolink', TRUE) && !$options['nolink']) {
				$options['nolink'] = _linodef_filter_option_nolink_auto($output);
			}
			// Finalize node processing.
			if (isset($options['nolink'])) {
				if (isset($options['attributes'])) {
					$output = _linodef_filter_option_attributes($output, $options['attributes']);
				}
				return $output;
			}
			else {
				if (isset($options['attributes'])) {
					$attributes = _linodef_filter_option_attributes_str2array($options['attributes']);
				}

				// Set the html tag class.
				$class = $fieldname ? 'linodef linodef_field linodef_field_' . $fieldname : 'linodef linodef_node linodef_node_' . $node->nid;
				if (isset($attributes['class'])) {
					$attributes['class'] = $attributes['class'] . ' ' . $class;
				}
				else {
					$attributes['class'] = $class;
				}

				// Create the link title.
				if (!isset($attributes['title'])) {
					$attributes['title'] = variable_get('linodef_' . $format . '_title_node', LINODEF_FORMAT_TITLE_NODE);
					// Debug: print('Title node <b>' . $node->nid . '</b> in format <b>' . $format . '</b>: raw: <b>' . $attributes['title'] . '</b>');
					if (module_exists('token') && variable_get('linodef_' . $format . '_title_node_token', 0)) {
						// Prevent multiple processing of the token module.
						if (linodef_process_running('token_replace')) {
							$attributes['title'] = preg_replace('/\[.*?\]/', '', $attributes['title']);
						}
						else {
							linodef_process_running('token_replace', 1);
							$attributes['title'] = token_replace($attributes['title'], 'node', $node);
							linodef_process_running('token_replace', 0);
						}
					}
					else {
						$linodef_tokens = linodef_tokens('node', $node);
						$attributes['title'] = str_ireplace(array_keys($linodef_tokens), array_values($linodef_tokens), $attributes['title']);
					}
					// Debug: print(' replaced: <b>' . $attributes['title'] . '</b><br />');
				}

				// Can cause bad HTML if field item contains html block elements. But Linodef shouldn't interfere with the content/formatting of the field.
				return l($output, 'node/'. $node->nid, array('html' => TRUE, 'attributes' => $attributes));
			}
		}
		else {
			// Access denied needed: Otherwise imagine the following: If a user writes a comment and tries to include e.g. [#NodeID] he would be able to see the title even if he have no proper access rights.
			return linodef_message(array('error' => array('node' => $node->nid, 'type' => 'access denied')));
		}
	}
	else {
		return linodef_message(array('error' => array('node' => $nid, 'type' => 'not found')));
	}
}

/**
 * Private function for _linodef_filter_process.
 *
 * @param $tid
 *   Term ID of the term that will be linked and embedded.
 * @param $options
 *   - no link, linktext
 * @return
 *   Returns a string containing the linked term name or a hint if term not found.
 */
function _linodef_find_term($tid, $format = -1, $options = array()) {
	if (module_exists('taxonomy')) {
		if ($tid && $term = taxonomy_get_term($tid)) {
			if (isset($options['nolink'])) {
				if (isset($options['attributes'])) {
					$output = '<span ' . $options['attributes'] . '>' . check_plain($term->name) . '</span>';
					return $output;
				}
				else {
					return check_plain($term->name);
				}
			}

			$termlink = taxonomy_term_path($term);
			if (isset($options['override'])) {
				$output = $options['override'];
			}
			else {
				$output = $term->name;
			}

			if (isset($options['attributes'])) {
				$attributes = _linodef_filter_option_attributes_str2array($options['attributes']);
			}

			// Set the html tag class.
			$class = 'linodef linodef_term linodef_term_' . $term->name;
			if (isset($attributes['class'])) {
				$attributes['class'] = $attributes['class'] . ' ' . $class;
			}
			else {
				$attributes['class'] = $class;
			}

			// Create the link title.
			if (!isset($attributes['title'])) {
				$attributes['title'] = variable_get('linodef_' . $format . '_title_term', LINODEF_FORMAT_TITLE_TERM);
				if (module_exists('token') && variable_get('linodef_' . $format . '_title_term_token', 0)) {
					// Prevent multiple processing of the token module.
					if (linodef_process_running('token_replace')) {
						$attributes['title'] = preg_replace('/\[.*?\]/', '', $attributes['title']);
					}
					else {
						linodef_process_running('token_replace', 1);
						$attributes['title'] = token_replace($attributes['title'], 'taxonomy', $term);
						linodef_process_running('token_replace', 0);
					}
				}
				else {
					$linodef_tokens = linodef_tokens('term', $term);
					$attributes['title'] = str_ireplace(array_keys($linodef_tokens), array_values($linodef_tokens), $attributes['title']);
				}
			}

			return l($output, $termlink, array('attributes' => $attributes));
		}
		else {
			return linodef_message(array('error' => array('term' => $tid, 'type' => 'not found')));
		}
	}
	else {
		return linodef_message(array('warning' => array('activate module' => 'Taxonomy'), 'explanation' => array('option to use in tag id' => array('%option' => 'tid', '%id' => $tid))));
	}
}

/**
 * Private function for _linodef_filter_process.
 *
 * @param $viewid
 *   View ID of the view that will be linked and embedded.
 * @param $options
 *   - no link, linktext
 *   - viewdisplay: Use a certain view display.
 *   - viewargs: Pass arguments to the view.
 *   - viewtitle: Include view title and link to the view.
 * @return
 *   Returns a string containing the linked view name or a message.
 */
function _linodef_find_view($viewid, $format = -1, $options = array()) {
	if (module_exists('views')) {
		if ($viewid && $view = views_get_view($viewid)) {
			$display = $options['display'] ? $options['display'] : 'default';
			$args = array();
			if (isset($options['arguments'])) {
				$args = explode(',', $options['arguments']);
				foreach ($args as &$value) {
					$value = trim($value);
				}
			}

			// Get the view display title, required for option content and link formatting below.
			$view_title = $view->get_title() ? $view->get_title() : t('view %viewname', array('%viewname' => $view->name));

			if (isset($options['override'])) {
				$output = $options['override'];
			}
			elseif ($options['content'] == 'title') {
				// Show view display title.
				$output = $view_title;
			}
			else {
				// Embed whole view.
				$output = $view->preview($display, $args);
			}
			// Set nolink automatically for content with HTML block level elements.
			if (variable_get('linodef_option_auto_nolink', TRUE) && !$options['nolink']) {
				$options['nolink'] = _linodef_filter_option_nolink_auto($output);
			}
			// Debug: print('(' . __FUNCTION__ . ') ');print('View id: <b>' . $viewid . '</b> Name: <b>' . $view->name . '</b> Display: <b>' . $display . '</b> Display path: <b>' . $view->display[$display]->display_options['path'] . '</b><pre>');print_r($options);print('</pre><hr />');
			// Create link if available/needed.
			if (!isset($options['nolink']) && !$view->disabled && ($path = $view->display[$display]->display_options['path'])) {
				$url_args = implode('/', $args);
				if ($url_args) {
					$path .= '/'. $url_args;
				}

				// Create the link title.
				$attributes = array('title' => variable_get('linodef_' . $format . '_title_view', LINODEF_FORMAT_TITLE_VIEW), 'class' => 'linodef linodef_view linodef_view_' . $view_title);
				$attributes['title'] = str_ireplace('[viewname]', $view_title, $attributes['title']);

				if (isset($options['attributes'])) {
					$attributes += _linodef_filter_option_attributes_str2array($options['attributes']);
				}
				return l($output, $view->get_url($args, $path), array('html' => TRUE, 'attributes' => $attributes));
			}
			else {
				if (isset($options['attributes'])) {
					$output = _linodef_filter_option_attributes($output, $options['attributes']);
				}
				return $output;
			}
		}
		else {
			return linodef_message(array('error' => array('view' => $viewid, 'type' => 'not found')));
		}
	}
	else {
		return linodef_message(array('warning' => array('activate module' => 'Views'), 'explanation' => array('option to use in tag id' => array('%option' => 'viewid', '%id' => $viewid))));
	}
}

/**
 * Private function for _linodef_filter_process.
 * This function is marked as deprecated. Option override offers this feature.
 *
 * @param $nid
 *   Node ID of the node that will be linked.
 * @param string $text
 *   The user entered text.
 * @return
 *   Returns a string containing the link or a hint if node not found.
 */
function _linodef_find_node($nid, $text) {
	$message = linodef_message(array('custom' => t('The tag %tag_deprecated is deprecated and replaced by option %option_new.', array('%tag_deprecated' => '&lt;#NodeID&gt;text&lt;/#&gt;', '%option_new' => 'override'))), 'status');
	if ($nid && $node = node_load($nid)) {
		if (node_access('view', $node)) {
			// l() does check_plain on $text and does any path aliases if required (thx @AjK).
			$output = l($text, 'node/'. $nid, array('attributes' => array('title' => $node->title)));
		}
		else {
			// Access denied not needed because the user without required rights won't see any content from the node he has no access to.
			$output = check_plain($text);
		}
	}
	else {
		$output = linodef_message(array('error' => array('node' => $nid, 'type' => 'not found'), 'custom' => t('desired to linking %usertext', array('%usertext' => $text))));
	}
	return $output . $message;
}

/**
 * Returns TRUE when a HTML block element is detected in $text.
 */
function _linodef_filter_option_nolink_auto($text) {
	$html_block_elements = array('<div', '<p', '<ol', '<ul', '<table', '<h1', '<h2', '<h3', '<dl', '<pre', '<h4', '<h5', '<h6', '<address', '<blockquote', '<fieldset', '<form', '<hr', '<noscript');
	foreach ($html_block_elements as $value) {
		if (strpos($text, $value) !== FALSE) {
			return TRUE;
			break;
		}
	}
	return NULL;
}

/**
 * Applies attributes to tags.
 */
function _linodef_filter_option_attributes($text, $attributes_string) {
	$html_block_elements = array('<div', '<p', '<ol', '<ul', '<table', '<h1', '<h2', '<h3', '<dl', '<pre', '<h4', '<h5', '<h6', '<address', '<blockquote', '<fieldset', '<form', '<hr', '<noscript');
	$html_inline_elements = array('<a', '<span', '<img', '<strong', '<em', '<code', '<del', '<ins', '<cite', '<object', '<sub', '<sup', '<bdo', '<map', '<area', '<param', '<q', '<kbd', '<var', '<acronym', '<abbr');
	$html_elements = $html_block_elements + $html_inline_elements;
	foreach ($html_elements as $value) {
		if (substr($text, 0, strlen($value)) == $value) {
			$text = substr_replace($text, $value . ' ' . $attributes_string, 0, strlen($value));
			$option_attributes_done = TRUE;
			break;
		}
	}
	// Use a div element if content contains block elements.
	if (!$option_attributes_done) {
		foreach ($html_block_elements as $value) {
			if (strpos($text, $value) !== FALSE) {
				$text = '<div ' . $attributes_string . '>' . $text . '</div>';
				$option_attributes_done = TRUE;
				break;
			}
		}
	}
	if (!$option_attributes_done) {
		$text = '<span ' . $attributes_string . '>' . $text . '</span>';
	}
	return $text;
}

/**
 * Str2array for option attributes.
 *
 * @param string $attributes_string
 * @return
 *   Returns an array with attributes to use with l().
 */
function _linodef_filter_option_attributes_str2array($attributes_string) {
	// Transform space inside attribute values.
	$attributes_string = preg_replace_callback("@(='[^']*')@", create_function('$treffer', 'return str_replace(" ", "\xf4\x80\x81\x8c", $treffer[0]);'), $attributes_string);
	// Several attributes separated by space.
	if (strpos($attributes_string, ' ') !== FALSE) {
		$attributes_array = explode(' ', $attributes_string);
		foreach ($attributes_array as $value) {
			if (($pos = strpos($value, '=')) !== FALSE) {
				$attributes_key = substr($value, 0, $pos);
				$value = substr($value, -(strlen($value)-$pos-1));
			}
			$attributes[$attributes_key] = $value;
		}
	}
	else {
		if (($pos = strpos($attributes_string, '=')) !== FALSE) {
			$attributes_key = substr($attributes_string, 0, $pos);
			$value = substr($attributes_string, -(strlen($attributes_string)-$pos-1));
		}
		$attributes[$attributes_key] = $value;
	}
	// Replace ' ' back and remove '' from value.
	foreach ($attributes as &$value) {
		if (is_string($value)) {
			$value = str_replace("\xf4\x80\x81\x8c", " ", $value);
			$value = str_replace("'", "", $value);
		}
	}
	unset($value);
	// Debug: print($attributes_string . '<pre>');print_r($attributes);print('</pre><br />');
	return $attributes;
}
