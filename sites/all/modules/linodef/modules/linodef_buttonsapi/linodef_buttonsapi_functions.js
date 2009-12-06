// $Id: linodef_buttonsapi_functions.js,v 1.1.2.1 2008/11/14 14:02:24 roidanton Exp $

/**
 * @file
 * Create a Linodef button with Javascript.
 */

/**
 * Create popup window. Link should direct to an url which is included by hook_theme()
 */
function linodef_buttonsapi_popup(link, width, height) {
    var w=window.open(link, 'linodef_popupwindow', 'width='+width+', height='+height+', scrollbars, resizable');
    w.focus();
}