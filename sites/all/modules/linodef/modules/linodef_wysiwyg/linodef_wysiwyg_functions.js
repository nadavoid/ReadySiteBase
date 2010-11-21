// $Id: linodef_wysiwyg_functions.js,v 1.1.2.1 2009/12/14 21:21:14 roidanton Exp $

/**
 * @file
 * Contains functions required by linodef_taglists.
 * Naming convention: modulename_functions.js
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Send the data to the correct textfield.
 * Window opener needed for Linodef popup.
 * Naming convention: modulename_onclick()
 *
 * @param string tag
 *   Contains the tag that will be inserted.
 * @param string instanceID
 *   The instanceID of the editor.
 */
function linodef_wysiwyg_onclick(tag, instanceID) {
  tag = tag.replace(/\xf4\x80\x81\x8c/gi, "\"");
  window.opener ? E = opener.Drupal.wysiwyg.instances[instanceID] : E = Drupal.wysiwyg.instances[instanceID];
  // Place cursor at the end of the tag.
  E.insert(tag);
}
