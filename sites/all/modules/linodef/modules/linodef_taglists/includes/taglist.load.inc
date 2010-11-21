<?php
// $Id: taglist.load.inc,v 1.1.2.1 2009/12/12 00:40:55 roidanton Exp $

/**
 * @file
 * Functions to retrieve the data for object lists.
 *
 * The admin provided data is filtered with check_plain and newlines are removed for compatibility with javascript of editors.
 *
 * @ingroup linodef_taglists
 * @author Roi Danton
 */

/**
 * Get data for button: Show elements by view name.
 *
 * @param string $viewname
 *      The name of the view.
 * @param string $options
 *      Linodef tag options separated by comma.
 * @param string $tl_properties
 *      Properties of the taglist.
 * @return
 *      array
 *          - data: the button list
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the element the data was retrieved from
 *          - element type
 *          - element
 *      string
 *          if view not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_getelementsbyviewname($viewname, $display_id = NULL, $args = NULL, $option_string, $tl_properties) {
	$view_result = _linodef_taglists_get_view_result($viewname, $display_id, $args);
	if (is_array($view_result)) {

		// Nodes/Fields.
		if (property_exists($view_result[0], nid)) {
			// Check for CCK fields: find the first field (lowest weight).
			$view_resultfields = get_object_vars($view_result[0]);
			while (current($view_resultfields) !== False && !$fieldvalue_key) {
				$view_resultkey = key($view_resultfields);
				if (substr($view_resultkey, 0, 15) == 'node_data_field') {
					// Create field name.
					$fieldname = strstr($view_resultkey, 'field_');
					$fieldname = substr($fieldname, 0, strrpos($fieldname, 'field_', 6)-1);
					// Key for the field value.
					if (substr($view_resultkey, -5) == 'value') {
						$fieldvalue_key = $view_resultkey;
					}
				}
				next($view_resultfields);
			}
			// Field syntax.
			if ($fieldvalue_key) {
				foreach ($view_result as $i => $item) {
					$item_vars = get_object_vars($item);
					if ($item_vars[$fieldvalue_key]) {
						$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $item_vars[$fieldvalue_key]);
						$output[$i] = array('tag' => '[#' . $item->nid .':'. $fieldname . $option_string . $comment . ']', 'desc' => $item_vars[$fieldvalue_key]);
					}
				}
			}
			// Default syntax (nodes).
			else {
				foreach ($view_result as $i => $item) {
					$node = node_load($item->nid);
					$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $node->title);
					$output[$i] = array('tag' => '[#' . $item->nid . $option_string . $comment . ']', 'desc' => $node->title);
				}
			}
		}

		// Terms.
		elseif (property_exists($view_result[0], tid)) {
			foreach ($view_result as $i => $item) {
				if (!$item->term_data_name) {
					$term = taxonomy_get_term($item->tid);
					$term_name = $term->name;
				}
				else {
					$term_name = $item->term_data_name;
				}
				$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $term_name);
				$output[$i] = array('tag' => '[#' . $item->tid . $option_string . $comment . ']', 'desc' => $term_name);
			}
		}
		return $output;
	}
	else {
		return $view_result;
	}
}

/**
 * Get data for button: Show fields by field name.
 *
 * @param string $fieldname
 *      The name of the field
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @param string $tl_properties
 *      Properties of the taglist.
 * @return
 *      array
 *          - data: the button list
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the element the data was retrieved from
 *      string
 *          if node/field (value) not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_getfieldsbyfieldname($fieldname, $option_string, $tl_properties) {
	// Heed content permissions settings if module is enabled.
	if (module_hook('content_permissions', perm)) {
		$content_perm = user_access('view '. $fieldname) ? True : False;
		if (!$content_perm) {
			return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'access denied')));
		}
	}
	$field = content_fields($fieldname);
	$fieldvalues = content_database_info($field);
	if ($fieldvalues['columns']['value']) {
		$i = 0;
		//$result = db_query('SELECT n.%s as value, n.* FROM {%s} n order by n.nid desc', $fieldvalues['columns']['value']['column'], $fieldvalues['table']);
		$result = db_query('SELECT n.%s as value, n.* FROM {%s} n order by n.%s %s', $fieldvalues['columns']['value']['column'], $fieldvalues['table'], $tl_properties['sort_method'], $tl_properties['sort_sequence']);
		while ($ergebnis = db_fetch_object($result)) {
			// Check if field has a value. If not, it doesn't appear.
			if ($ergebnis->value) {
				if ($ergebnis->nid && $node = node_load($ergebnis->nid)) {
					if (node_access('view', $node)) {
						// Remove all Linodef tags as they are not embedded by filter, too.
						$ergebnis->value = linodef_removetags($ergebnis->value);
						// Security and compatibility step.
						$ergebnis->value = check_plain($ergebnis->value);
						$node->title = check_plain($node->title);
						// Set default comment. Use node titles since field content could contain new lines.
						$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $node->title);
						// Make compatible with editor's Javascripts (remove newline). If not done with drupal_to_js() in editorbuttons submodules.
						$ergebnis->value = str_replace(array("\r\n", "\n", "\r"), ' ', $ergebnis->value);
						// Multiple values: only insert valuenumber if fieldvalue is not first value.
						if ($ergebnis->delta && $ergebnis->delta > 0) {
							$output[$i] = array('tag' => '[#' . $ergebnis->nid . ':' . $fieldname . ':' . $ergebnis->delta . $option_string . $comment . ']', 'desc' => $ergebnis->value . ' (' . $node->title . ')');
						}
						else {
							$output[$i] = array('tag' => '[#' . $ergebnis->nid . ':' . $fieldname . $option_string . $comment . ']', 'desc' => $ergebnis->value .' (' . $node->title . ')');
						}
					}
					else {
						if (variable_get('linodef_message_level', 'all') != 'critical') {
							$output[$i] = array('tag' => '', 'desc' => linodef_message(array('error' => array('node' => $ergebnis->nid, 'type' => 'access denied')), NULL, FALSE));
						}
					}
					$i++;
				}
			}
		}
		if (!is_array($output)) {
			return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'no content')), 'error');
		}
		return $output;
	}
	elseif ($fieldvalues['columns']['nid']) {
		return linodef_message(
			array(
				'custom' => t('Reference fields are not supported.'),
				'warning' => array('supported field types' => TRUE),
				'custom' => t('Instead of this fieldname the usage of a content type is recommended')
			),
			'error'
		);
	}
	else {
		return linodef_message(
			array(
				'error' => array('field' => $fieldname, 'type' => 'not found'),
				'custom' => t('or is an unsupported field type.'),
				'warning' => array('supported field types' => TRUE),
			)
		);
	}
}

/**
 * Get data for button: Show nodes by content type
 *
 * @param string $contenttype
 *      The content type to get the nodes from.
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @param string $tl_properties
 *      Properties of the taglist.
 * @return
 *      array
 *          - data: the button list
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the element the data was retrieved from
 *      string
 *          if content type not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_getnodesbycontenttype($contenttype, $option_string, $tl_properties) {
	if ($ctype_obj = node_get_types('type', $contenttype)) {
		$i = 0;
		//$result = db_query('SELECT n.nid FROM {node} n WHERE n.type = "%s"', $contenttype);
		$result = db_query('SELECT n.nid FROM {node} n WHERE n.type = "%s" order by n.%s %s', $contenttype, $tl_properties['sort_method'], $tl_properties['sort_sequence']);
		while ($ergebnis = db_fetch_object($result)) {
			if ($ergebnis->nid && $node = node_load($ergebnis->nid)) {
				if (node_access('view', $node)) {
					$node->title = check_plain($node->title);
					$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $node->title);
					$output[$i] = array('tag' => '[#' . $node->nid . $option_string . $comment . ']', 'desc' => $node->title);
				}
				else {
					if (variable_get('linodef_message_level', 'all') != 'critical') {
							$output[$i] = array('tag' => '', 'desc' => linodef_message(array('error' => array('node' => $ergebnis->nid, 'type' => 'access denied')), NULL, FALSE));
					}
				}
				$i++;
			}
		}
		if (!is_array($output)) {
			return linodef_message(array('error' => array('content type' => $contenttype, 'type' => 'no content')), 'error');
		}
		//$output['name'] = $ctype_obj->name;
		return $output;
	}
	else {
		return linodef_message(array('error' => array('content type' => $contenttype, 'type' => 'not found')));
	}
}

/**
 * Get data for button: Show terms by vocabulary.
 *
 * @param string $vid
 *      The ID of the vocabulary to get the terms from.
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @param string $tl_properties
 *      Properties of the taglist.
 * @return
 *      array
 *          - data: the button list
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the element the data was retrieved from
 *      string
 *          if vid not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_gettermsbyvid($vid, $option_string, $tl_properties) {
	if (module_exists('taxonomy')) {
		if ($terms = taxonomy_get_tree($vid)) {
			for ($i = 0; $i < count($terms); $i++) {
				$comment = _linodef_taglists_setdefaultcomment($tl_properties['comment'], $terms[$i]->name);
				$output[$i] = array('tag' => '[#' . $terms[$i]->tid . $option_string . $comment . ']', 'desc' => $terms[$i]->name);
			}
			if (!is_array($output)) {
				return linodef_message(array('error' => array('vocabulary' => $vid, 'type' => 'no content')), 'error');
			}
			//$vocab_obj = taxonomy_vocabulary_load($vid);
			//$output['name'] = $vocab_obj->name;
			return $output;
		}
		else {
			return linodef_message(array('error' => array('vocabulary' => $vid, 'type' => 'not found')));
		}
	}
	else {
		return linodef_message(array('warning' => array('activate module' => 'Taxonomy'), 'explanation' => array('option to use in tag id' => array('%option' => 'tid', '%id' => $tid))));
	}
}

/**
 * Get data for button: List all views.
 *
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @param string $tl_properties
 *      Properties of the taglist.
 * @return
 *      array
 *          - data: the button list
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the element the data was retrieved from
 *          - element type
 *          - element
 *      string
 *          if vid not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_getallviews($option_string, $tl_properties) {
	if (module_exists('views')) {
		$output = array();
		$all_views = views_get_all_views();
		foreach ($all_views as $i => $item) {
				$output[$i] = array('tag' => '[#' . $item->name . $option_string . $comment . ']', 'desc' => $item->name);
		}
		return $output;
	}
	else {
		return linodef_message(array('warning' => array('activate module' => 'Views')));
	}
}

/**
 * Helper function for _linodef_taglists_getelementssbyviewname().
 * Investigate the result of a view.
 *
 * @param string $viewname
 *      The name of the view to retrieve the data from.
 * @param string $display_id
 *      The display id. On the edit page for the view in question, you'll find
 *      a list of displays at the left side of the control area. "Defaults"
 *      will be at the top of that list. Hover your cursor over the name of the
 *      display you want to use. An URL will appear in the status bar of your
 *      browser. This is usually at the bottom of the window, in the chrome.
 *      Everything after #views-tab- is the display ID, e.g. page_1.
 * @param array $args
 *      Array of arguments.
 * @return
 *      array
 *          An array containing an object for each view item. For details see http://drupal.org/node/342132 and http://views.doc.logrus.com/classview.html.
 *      string
 *          If the view is not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 */
function _linodef_taglists_get_view_result($viewname, $display_id = NULL, $args = NULL) {
	if (module_exists('views')) {
		$view = views_get_view($viewname);
		if (is_object($view)) {
			if (is_array($args)) {
				$view->set_arguments($args);
			}
			if (is_string($display_id)) {
				$view->set_display($display_id);
			}
			else {
				// Heed pager setting.
				$view->init_display();
			}
			$view->pre_execute();
			$view->execute();
			return $view->result;
		}
		else {
			return linodef_message(array('error' => array('view' => $viewname, 'type' => 'not found')));
		}
	}
	else {
		return linodef_message(array('warning' => array('activate module' => 'Views'), 'explanation' => array('option to use in tag id' => array('%option' => 'viewid', '%id' => $viewname))));
	}
}

/**
 * Returns the content or an empty string.
 */
function _linodef_taglists_setdefaultcomment($active, $content) {
	return $active ? ' ' . str_replace(']', '', $content) : '';
}