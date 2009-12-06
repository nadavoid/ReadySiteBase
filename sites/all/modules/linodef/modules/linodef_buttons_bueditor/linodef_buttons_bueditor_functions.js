// $Id: linodef_buttons_bueditor_functions.js,v 1.1.2.2 2008/11/30 13:00:30 roidanton Exp $

/**
 * @file
 * Create the Linodef BUEditor buttons with Javascript.
 *
 * The name of this file has to be the modules name & _functions: modulename_functions.js.
 *
 * @package Linodef
 * @author Roi Danton
 */

/**
 * Create BUEditor quickPop with content.
 */
function linodef_buttons_bueditor_quickpop(content, effect) {
  BUE.quickPop.open(eDefHTML('div', content, {'class': 'chooser'}), effect);
}

/**
 * Passes the data to the correct textfield and focus' this.
 * Window opener needed for Linodef popup.
 *
 * Naming convention: modulename_onclick()
 * Parameter convention: Only one parameter allowed that contains the tag.
 */
function linodef_buttons_bueditor_onclick(tag) {
  window.opener ? E = opener.BUE.active : E = BUE.active;
  // Place cursor at the end of the tag.
  E.replaceSelection(tag, 'end');
  E.focus();
}

/**
 * Create BUEditor dialog with content.
 * Reserved for future use.
function linodef_buttons_bueditor_dialog(title, content, effect) {
  BUE.dialog.open(title, content, effect);
}
*/
/**
 * Retrieve id of active textarea, useful for ahah in Drupals FAPI.
 * Reserved for future use.
function linodef_buttons_bueditor_gettextareaid() {
    return BUE.active.textArea.id;
}
 */
/**
 * Create jQuery UI popup dialog.
 * jQuery UI required, documentation and demos of dialog: http://dev.jquery.com/view/trunk/ui/demos/functional/#ui.dialog
 * Not senseful since it supports no scrolling of body. -> Removed
 */