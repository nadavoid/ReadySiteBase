<?php
// $Id: linodef-filter.inc,v 1.1.2.4 2008/11/30 13:00:30 roidanton Exp $

/**
 * @file
 * The replace functions executed in the process filter step.
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Return body with replaced tags.
 *
 * Search and replace the tags with internal links and node title or field values.
 *
 * @param $body
 *   Text on that the preg_replace function will be executed.
 */
function _linodef_filter_process(&$body) {

  // Define Linodef tags
  $preg = array(

    // Link Node ID with user defined text
    "@\xf4\x80\x81\x8c#([0-9]+)\xf4\x80\x8f\x8d(.*?)\xf4\x80\x81\x8c/#\xf4\x80\x8f\x8d@se"                => "_linodef_find_node($1,'$2');",

    // Link Node ID and embed title or field values
    "@\[#([0-9]+)\]@e"                                      => "_linodef_find_nodesnfields($1);",
    "@\[#([0-9]+):([0-9a-z_]+)\]@e"                         => "_linodef_find_nodesnfields($1,$2);",
    "@\[#([0-9]+):([0-9a-z_]+):([0-9]+)\]@e"                => "_linodef_find_nodesnfields($1,$2,$3);",

    // Link Node ID and embed title or field values, comment has to be substituted too
    "@\[#([0-9]+) (.+?)\]@e"                                => "_linodef_find_nodesnfields($1);",
    "@\[#([0-9]+):([0-9a-z_]+) (.+?)\]@e"                   => "_linodef_find_nodesnfields($1,$2);",
    "@\[#([0-9]+):([0-9a-z_]+):([0-9]+) (.+?)\]@e"          => "_linodef_find_nodesnfields($1,$2,$3);",

    // ID with options
    "@\[#([0-9]+),([a-zA-Z,=]*)\]@e"                                   => "_linodef_filter_processoptions($1,False,0,'$2');",
    "@\[#([0-9]+):([0-9a-z_]+),([a-zA-Z,=]*)\]@e"                      => "_linodef_filter_processoptions($1,$2,0,'$3');",
    "@\[#([0-9]+):([0-9a-z_]+):([0-9]+),([a-zA-Z,=]*)\]@e"             => "_linodef_filter_processoptions($1,$2,$3,'$4');",

    // ID with options, comment has to be substituted too
    "@\[#([0-9]+),([a-zA-Z,=]*) (.+?)\]@e"                             => "_linodef_filter_processoptions($1,False,0,'$2');",
    "@\[#([0-9]+):([0-9a-z_]+),([a-zA-Z,=]*) (.+?)\]@e"                => "_linodef_filter_processoptions($1,$2,0,'$3');",
    "@\[#([0-9]+):([0-9a-z_]+):([0-9]+),([a-zA-Z,=]*) (.+?)\]@e"       => "_linodef_filter_processoptions($1,$2,$3,'$4');",

  );
  $body = preg_replace(array_keys($preg), array_values($preg), $body);

  return $body;
}

function _linodef_filter_processoptions($id, $fieldname = False, $valuenumber = 0, $option_string = '') {
    $type = 'nid';
    $message = '';
    if (!empty($option_string) && $options = explode(',', $option_string)) {
        // Get and validate type: Last occurence of tid or nid determines type (if user does the fault including both).
        foreach ($options as $value) {
            if ($value == 'nid') {
                $type = 'nid';
            }
            elseif ($value == 'tid') {
                if (!$fieldname) {
                    $type = 'tid';
                }
                else {
                    $message .= linodef_message(array('warning' => array('option ignored' => array('%id' => $id, '%option' => $value)), 'custom' => t('since a fieldname is set and therefore the node ID is used instead of the term ID')), TRUE);
                }
            }
        }
        // Get and validate options.
        foreach ($options as $value) {
            if ($value == 'nolink') {
                $options['no_link'] = TRUE;
            }
            elseif (substr($value, 0, 11) == 'translation') {
                if ($type != 'tid') {
                    $options['translation'] = substr($value, 12, 2);
                }
                else {
                    $message .= linodef_message(array('warning' => array('option ignored' => array('%id' => $id, '%option' => $value)), 'custom' => t('since a taxonomy term does not use content translation')), TRUE);
                }
            }
            elseif ($value == '') {
                $message .= linodef_message(array('custom' => t('Enter a further option for Linodef tag with id %id or remove the last comma inside the tag', array('%id' => $id))), TRUE, 'warning');
            }
            elseif ($value != 'nid' && $value != 'tid') {
                $message .= linodef_message(array('warning' => array('option unknown' => array('%id' => $id, '%option' => $value))), TRUE);
            }
        }
    }
    else {
        $message .= linodef_message(array('custom' => t('Enter an option for Linodef tag with id %id or remove the comma inside the tag', array('%id' => $id))), TRUE, 'warning');
        $options = array();
    }
    switch ($type) {
        case 'nid':
            return _linodef_find_nodesnfields($id, $fieldname, $valuenumber, $options) . $message .' ';
            break;
        case 'tid':
            return _linodef_find_term($id, $options) . $message .' ';
            break;
    }
}

/**
 * Helper function for _linodef_filter_process.
 *
 * @param $nid
 *   Node ID of the node that will be linked.
 * @param string $text
 *   The user entered text.
 * @return
 *   Returns a string containing the link or a hint if node not found.
 */
function _linodef_find_node($nid, $text) {
  if ($nid && $node = node_load($nid)) {
    if (node_access('view', $node)) {
        // l() does check_plain on $text and does any path aliases if required (thx @AjK).
        return l($text, 'node/'. $nid, array('attributes' => array('title' => $node->title)));
    }
    else {
        // Access denied not needed because the user without required rights won't see any content from the node he has no access to.
        return check_plain($text);
    }
  }
  return linodef_message(array('error' => array('node' => $nid, 'type' => 'not found'), 'custom' => t('desired to linking %usertext', array('%usertext' => $text))), TRUE);
}

/**
 * Helper function for _linodef_filter_process.
 *
 * @param $nid
 *   Node ID of the node that will be linked.
 * @param $fieldname
 *   The name of the field reading the value of or "title" (to include nodetitle) if not set.
 * @param $valuenumber
 *   Number to get the desired value of multiple value fields or 0 if not set.
 * @param $options
 *      - no_link: If true no link will be added to the embedded text.
 *      - translation: If set the function will check if the node has a translated node in the language this value has (e.g. de, en, es etc).
 * @return
 *   Returns a string containing the (linked) node title/field value or a hint if node/field/field-content not found.
 */
function _linodef_find_nodesnfields($nid, $fieldname = False, $valuenumber = 0, $options = array()) {
  if ($nid && $node = node_load($nid)) {
    // If translation option is set, find translated node with desired language. If no node in that language is available continue as usual.
    if (isset($options['translation']) && $language = $options['translation']) {
        if (module_exists('translation')) {
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
            return linodef_message(array('warning' => array('activate module' => 'content translation'), 'custom' => t('to use the option %option of Linodef tag with id %nid', array('%option' => 'translation', '%nid' => $nid))), TRUE);
        }
    }
    if (node_access('view', $node)) {
        if ($fieldname == False) {
            $output = $node->title;
        }
        elseif (property_exists($node, $fieldname)) {
            // Heed content permissions settings if module is enabled.
            if (module_exists('content_permissions')) {
                $content_perm = user_access('view '. $fieldname) ? TRUE : FALSE;
                if (!$content_perm) {
                        return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'access denied')), TRUE);
                }
            }
            $output = get_object_vars($node);
            $output = $output[$fieldname][$valuenumber]['value'];
            if (drupal_strlen($output) < 1) {
                $erroneous_tag[0] = '#'. $nid .':'. $fieldname;
                // Set correct tag layout for error message heeding the translation option.
                if ($options['translation'] && $nid != $node->nid) {
                    $erroneous_tag[2] = ',translation='. $options['translation'];
                }
                if ($valuenumber == 0) {
                    $erroneous_tag_str = ksort($erroneous_tag) ? implode($erroneous_tag) : '';
                    return linodef_message(array('custom' => t('Field %field of node %nid has no content and hence cannot be embedded. Create content for that field or revise the Linodef tag which contains %erroneous_tag', array('%field' => $fieldname, '%nid' => $node->nid, '%erroneous_tag' => $erroneous_tag_str))), TRUE, 'error');
                }
                else {
                    $erroneous_tag[1] = ':'. $valuenumber;
                    $erroneous_tag_str = ksort($erroneous_tag) ? implode($erroneous_tag) : '';
                    return linodef_message(array('custom' => t('Value %valuenumber of field %field of node %nid not found or has no content and hence cannot be embedded. Create content for that field or revise the Linodef tag which contains %erroneous_tag', array('%valuenumber' => $valuenumber, '%field' => $fieldname, '%nid' => $node->nid, '%erroneous_tag' => $erroneous_tag_str))), TRUE, 'error');
                }
            }
        }
        else {
            return linodef_message(array('error' => array('field' => $fieldname, 'type' => 'not found'), 'custom' => t('or is not assigned to content type of node %nid', array('%nid' => $nid))), TRUE);
        }

        // Remove existing Linodef tags before doing markup to avoid self and cross referencing (causing an infinite recursion) until unlimited depths.
        $output = linodef_removetags($output);
        // Remove self referencing tags causing an infinite recursion and thus white page. Not needed, recursion removed.
        // $output = str_replace('[#'. $nid .':'. $fieldname .']', '', $output);
        // How to remove cross referencing tags causing an infinite recursion and thus white page? Possible till a depth of 2 with variable_set, but cross ref still possible with deeper depth. Therefore recursion removed.

        // Convert the tags of all the filters of default input filter.
        // Include a filter settings so admin can choose if he wants to use plain text with check_plain? Not needed IMHO (keep it simple).
        $output = check_markup($output, '', False);
        // Remove annoying <p> tag enclosing $output created by line break converter.
        if (substr($output, 0, 3) == '<p>') {
            $output = substr($output, 3);
        }
        if (substr($output, -5, 4) == '</p>') {
            $output = substr($output, 0, -5);
        }

        if (isset($options['no_link'])) {
            return $output;
        }
        else {
            // l() does check_plain on $text and does any path aliases if required (thx @AjK). html => true let l() doesn't do check_plain.
            return l($output, 'node/'. $node->nid, array('html' => true, 'attributes' => array('title' => t('Node') .' '. $node->nid .': '. $node->title)));
        }
    }
    else {
        // Access denied needed: Otherwise imagine the following: If a user writes a comment and tries to include e.g. [#NodeID] he would be able to see the title even if he have no proper access rights.
        return linodef_message(array('error' => array('node' => $node->nid, 'type' => 'access denied')), TRUE);
    }
  }
  else {
    return linodef_message(array('error' => array('node' => $nid, 'type' => 'not found')), TRUE);
  }
}

/**
 * Helper function for _linodef_filter_process.
 *
 * @param $tid
 *   Term ID of the term that will be linked and embedded.
 * @param $options
 *      - no_link: If true no link will be added to the embedded text.
 * @return
 *   Returns a string containing the linked term name or a hint if term not found.
 */
function _linodef_find_term($tid, $options = array()) {
    if ($tid && $term = taxonomy_get_term($tid)) {
        if (isset($options['no_link'])) {
            return check_plain($term->name);
        }
        $termlink = taxonomy_term_path($term);
        return l($term->name, $termlink, array('attributes' => array('title' => t('Shows a teaser list of nodes using term @term', array('@term' => $term->name)))));
    }
    else {
        return linodef_message(array('error' => array('term' => $tid, 'type' => 'not found')), TRUE);
    }
}