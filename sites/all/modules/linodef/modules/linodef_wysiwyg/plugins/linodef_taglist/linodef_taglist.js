// $Id: linodef_taglist.js,v 1.1.2.3 2009/12/15 12:28:18 roidanton Exp $

/**
 * Don't use JSON since we mustn't have a singleton here.
 */
function linodef_wysiwyg_taglist() {
	/**
	 * Return whether the passed node belongs to this plugin.
	 *
	 * @param node
	 *   The currently focused DOM element in the editor content.
	 */
	this.isNode = function (node) {
		return $(node).is('.linodef');
	},

	/**
	 * Execute the button.
	 *
	 * @param data
	 *   An object containing data about the current selection:
	 *   - format: 'html' when the passed data is HTML content, 'text' when the
	 *     passed data is plain-text content.
	 *   - node: When 'format' is 'html', the focused DOM element in the editor.
	 *   - content: The textual representation of the focused/selected editor
	 *     content.
	 * @param settings
	 *   The plugin settings, as provided in the plugin's PHP include file.
	 * @param instanceId
	 *   The ID of the current editor instance.
	 */
	this.invoke = function(data, settings, instanceId) {
		// Generate HTML markup.
		/*if (data.format == 'html') {
		}
		// Generate plain text.
		else {
		}*/
		// Appending the instanceId to the path and open the popup.
		linodef_taglists_popup(settings.dialog.url + '/' + instanceId, settings.dialog.width, settings.dialog.height);
		// Insert new content into the editor.
		/*if (typeof content != 'undefined') {
			Drupal.wysiwyg.instances[instanceId].insert(content);
			//Drupal.wysiwyg.instances[instanceId].openDialog(settings.dialog);
		}*/
	}
};

/**
 * Add taglist to wysiwyg plugin object.
 *
 * Has to be done with inline call of this function since
 * the preferred method of using a Drupal namespace
 * (e.g. Drupal.settings or an own) does not work. The
 * variable is initialized after the loading of JS files.
 * @param tlid
 *   The taglist id.
 */
function linodef_wysiwyg_add_taglist(tlid) {
	// Array notation to insert id.
	Drupal.wysiwyg.plugins['linodef_taglist_'+tlid] = new linodef_wysiwyg_taglist();
}
