// $Id: linodef_taglists_functions.js,v 1.1.2.1 2009/12/12 00:40:55 roidanton Exp $

/**
 * @file
 * Opens a popup with Javascript.
 *
 * @ingroup linodef_taglists
 */

/**
 * Create popup window. Link should direct to an url which is included by hook_theme()
 */
function linodef_taglists_popup(link, width, height) {
    var w=window.open(link, 'linodef_popupwindow', 'width='+width+', height='+height+', scrollbars, resizable');
    w.focus();
}