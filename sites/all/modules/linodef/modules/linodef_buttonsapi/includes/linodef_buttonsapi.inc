<?php
// $Id: linodef_buttonsapi.inc,v 1.1.2.4 2008/11/30 13:54:07 roidanton Exp $

/**
 * @file
 * Helper functions to retrieve the data for the buttons.
 *
 * The user data is filtered with check_plain and newlines are removed for compatibility with javascript of editors.
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Get data for button: Show fields by field name.
 *
 * @param string $fieldname
 *      The name of the field
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @return
 *      array
 *          - data: the node/field/vocab data
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the node/field/vocab
 *          - element type
 *          - element
 *      string
 *          if node/field (value) not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 *
 * @see linodef_editorbuttons_switch()
 */
function _linodef_buttonsapi_getfieldsbyfieldname($fieldname, $option_string) {
    // Heed content permissions settings if module is enabled.
    if (module_hook('content_permissions', perm)) {
        $content_perm = user_access('view '. $fieldname) ? True : False;
        if (!$content_perm) {
                return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'access denied')), TRUE);
        }
    }
    $field = content_fields($fieldname);
    $fieldvalues = content_database_info($field);
    if ($fieldvalues['columns']['value']) {
        $output = array();
        $i = 0;
        //$result = db_query('SELECT n.%s as value, n.* FROM {%s} n order by n.nid desc', $fieldvalues['columns']['value']['column'], $fieldvalues['table']);
        $result = db_query('SELECT n.%s as value, n.* FROM {%s} n order by n.%s %s', $fieldvalues['columns']['value']['column'], $fieldvalues['table'], variable_get('linodef_buttonsapi_itemsort_field_method', 'vid'), variable_get('linodef_buttonsapi_itemsort_field_sequence', 'desc'));
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
                        // Set default comment.
                        $comment = variable_get('linodef_buttonsapi_comments', 0) ? ' '. str_replace(']', '', $node->title) : '';
                        // Make compatible with editor's Javascripts (remove newline). If not done with drupal_to_js() in editorbuttons submodules.
                        $ergebnis->value = str_replace(array("\r\n", "\n", "\r"), ' ', $ergebnis->value);
                        // Multiple values: only insert valuenumber if fieldvalue is not first value.
                        if ($ergebnis->delta && $ergebnis->delta > 0) {
                            $output['data'][$i] = array('tag' => '[#'. $ergebnis->nid .':'. $fieldname .':'. $ergebnis->delta . $option_string . $comment .']', 'desc' => $ergebnis->value .' ('. $node->title .')');
                        }
                        else {
                            $output['data'][$i] = array('tag' => '[#'. $ergebnis->nid .':'. $fieldname . $option_string . $comment .']', 'desc' => $ergebnis->value .' ('. $node->title .')');
                        }
                    }
                    else {
                        if (variable_get('linodef_message_level', 'all') != 'critical') {
                            $output['data'][$i] = array('tag' => '', 'desc' => linodef_message(array('error' => array('node' => $ergebnis->nid, 'type' => 'access denied'))));
                        }
                    }
                    $i++;
                }
            }
        }
        if (!is_array($output['data'])) {
            return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'no content')), TRUE, 'error');
        }
        // Retrieving field labels.
        // view $content_type['type'] doesn't exist yet in default Drupal permissions.
        if (user_access('view content')) {
            $content_type_info = _content_type_info();
            foreach ($content_type_info['content types'] as $content_type) {
                if (isset($content_type['fields'][$fieldname])) {
                    $labels[] .= $content_type['name'] .': '. $content_type['fields'][$fieldname]['widget']['label'];
                }
            }
        }
        $output['name'] = is_array($labels) ? implode(', ', $labels) : NULL;
        $output['element type']['input'] = t('field');
        $output['element type']['output'] = t('fields');
        $output['element'] = $fieldname;
        return $output;
    }
    elseif ($fieldvalues['columns']['nid']) {
        return linodef_message(array('custom' => t('Reference fields are not supported.'), 'warning' => array('supported field types' => TRUE), 'custom' => t('Instead of this fieldname the usage of a content type is recommended')), TRUE, 'error');
    }
    else {
        return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'not found'), 'custom' => t('or is an unsupported field type.'), 'warning' => array('supported field types' => TRUE), 'warning' => array('check variable' => '$drupal_element')), TRUE);
    }
}

/**
 * Get data for button: Show nodes by content type
 *
 * @param string $contenttype
 *      The content type to get the nodes from.
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @return
 *      array
 *          - data: the node/field/vocab data
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the node/field/vocab
 *          - element type
 *          - element
 *      string
 *          if content type not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 *
 * @see linodef_editorbuttons_switch()
 */
function _linodef_buttonsapi_getnodesbycontenttype($contenttype, $option_string) {
    if ($ctype_obj = node_get_types('type', $contenttype)) {
        $output = array();
        $i = 0;
        //$result = db_query('SELECT n.nid FROM {node} n WHERE n.type = "%s"', $contenttype);
        $result = db_query('SELECT n.nid FROM {node} n WHERE n.type = "%s" order by n.%s %s', $contenttype, variable_get('linodef_buttonsapi_itemsort_node_method', 'title'), variable_get('linodef_buttonsapi_itemsort_node_sequence', 'asc'));
        while ($ergebnis = db_fetch_object($result)) {
            if ($ergebnis->nid && $node = node_load($ergebnis->nid)) {
                if (node_access('view', $node)) {
                    $node->title = check_plain($node->title);
                    // Set default comment.
                    $comment = variable_get('linodef_buttonsapi_comments', 0) ? ' '. str_replace(']', '', $node->title) : '';
                    $output['data'][$i] = array('tag' => '[#'. $node->nid . $option_string . $comment .']', 'desc' => $node->title);
                }
                else {
                    if (variable_get('linodef_message_level', 'all') != 'critical') {
                        $output['data'][$i] = array('tag' => '', 'desc' => linodef_message(array('error' => array('node' => $ergebnis->nid, 'type' => 'access denied'))));
                    }
                }
                $i++;
            }
        }
        if (!is_array($output['data'])) {
            return linodef_message(array('error' => array('content type' => $contenttype, 'type' => 'no content')), TRUE, 'error');
        }
        $output['name'] = $ctype_obj->name;
        $output['element type']['input'] = t('content type');
        $output['element type']['output'] = t('nodes');
        $output['element'] = $contenttype;
        return $output;
    }
    else {
        return linodef_message(array('error' => array('content type' => $contenttype, 'type' => 'not found'), 'warning' => array('check variable' => '$drupal_element')), TRUE);
    }
}

/**
 * Get data for button: Show terms by vocabulary.
 *
 * @param string $vid
 *      The ID of the vocabulary to get the terms from.
 * @param string $option_string
 *      Linodef tag options separated by comma.
 * @return
 *      array
 *          - data: the node/field/vocab data
 *              - tag: the tag to be inserted
 *              - desc: the description for the editor button
 *          - name: the label/name of the node/field/vocab
 *          - element type
 *          - element
 *      string
 *          if vid not found a message is returned which should appear in editor text field (depends on Linodef submodule for editor)
 *
 * @see linodef_editorbuttons_switch()
 */
function _linodef_buttonsapi_gettermsbyvid($vid, $option_string) {
    if ($terms = taxonomy_get_tree($vid)) {
        $output = array();
        for ($i = 0; $i < count($terms); $i++) {
            // Set default comment.
            $comment = variable_get('linodef_buttonsapi_comments', 0) ? ' '. str_replace(']', '', $terms[$i]->name) : '';
            $output['data'][$i] = array('tag' => '[#'. $terms[$i]->tid .',tid'. $option_string . $comment .']', 'desc' => $terms[$i]->name);
        }
        if (!is_array($output['data'])) {
            return linodef_message(array('error' => array('vocabulary' => $vid, 'type' => 'no content')), TRUE, 'error');
        }
        $vocab_obj = taxonomy_vocabulary_load($vid);
        $output['name'] = $vocab_obj->name;
        $output['element type']['input'] = t('vocabulary');
        $output['element type']['output'] = t('terms');
        $output['element'] = $vid;
        $output['options']['tid'] = 'tid';
        return $output;
    }
    else {
        return linodef_message(array('error' => array('vocabulary' => $vid, 'type' => 'not found'), 'warning' => array('check variable' => '$drupal_element')), TRUE);
    }
}