// $Id: filefield_insert.js,v 1.5 2009/06/15 16:18:36 quicksketch Exp $

/**
 * Behavior to add "Insert" buttons to FileField uploads.
 */
Drupal.behaviors.fileFieldInsert = function(context) {
  if (typeof(fileFieldInsertTextarea) == 'undefined') {
    fileFieldInsertTextarea = false;
  }

  // Keep track of the last active textarea (if not using WYSIWYG).
  $('.node-form textarea:not([name$="[data][title]"])', context).focus(fileFieldInsertSetActive).blur(fileFieldInsertRemoveActive);

  // Add the insert widget, replacing the 'filefield-insert-location' element.
  $('div.filefield-element', context).each(function() {
    if ($(this).find('input[name$="[fid]"]').val() > 0) {
      var widget = $('input.filefield-insert-widget', this).val();
      var $location = $('input.filefield-insert-location:first', this);
      var $insert = $(Drupal.settings.fileFieldInsert.widgets[widget]).find('input.filefield-insert').click(fileFieldInsert).end();
      $location.replaceWith($insert);
    }
  });

  function fileFieldInsertSetActive() {
    fileFieldInsertTextarea = this;
    this.fileFieldInsertHasFocus = true;
  }

  function fileFieldInsertRemoveActive() {
    if (fileFieldInsertTextarea == this) {
      var thisTextarea = this;
      setTimeout(function() {
        thisTextarea.fileFieldInsertHasFocus = false;
      }, 1000);
    }
  }

  function fileFieldInsert() {
    var $fileFieldElement = $(this).parents('div.filefield-element:first');
    var $description = $fileFieldElement.find('input[name$="[description]"]:first');
    var $alt = $fileFieldElement.find('input[name$="[alt]"]:first');
    var $title = $fileFieldElement.find('input[name$="[title]"]:first, textarea[name$="[title]"]:first');
    var $preset = $fileFieldElement.find('select.filefield-insert-preset:first');

    var widget = $fileFieldElement.find('input.filefield-insert-widget').val();
    var url = $fileFieldElement.find('input.filefield-insert-url').val();
    var width = parseInt($fileFieldElement.find('input.filefield-insert-width').val());
    var height = parseInt($fileFieldElement.find('input.filefield-insert-height').val());
    var maxWidth = parseInt($fileFieldElement.find('input.filefield-insert-max-width').val());
    var extension = url.replace(/.*?\.([a-z0-9]+)$/i, '$1').toLowerCase();
    var filename = url.replace(/.*?\/([^\/]+)$/i, '$1');
    var description = $description.size() ? $description.val() : '';
    var alt = $alt.size() ? $alt.val() : '';
    var title = $title.size() ? $title.val() : '';
    var preset = $preset.size() ? $preset.val() : '';
    var fileDir = Drupal.settings.fileFieldInsert.fileDirectoryPath;
    var content = '';

    // Update the URL if using a preset.
    if (preset) {
      var pieces = preset.split('|');
      url = url.replace(fileDir + '/', fileDir + '/imagecache/' + pieces[0] + '/');
      // If the ImageCache preset specified both height and width.
      if (pieces[1] != '0' && pieces[2] != '0') {
        width = parseInt(pieces[1]);
        height = parseInt(pieces[2]);
      }
      // If just the width, calculate the height.
      else if (pieces[1] != '0') {
        var newWidth = parseInt(pieces[1]);
        var newRatio = newWidth / width;
        width = newWidth;
        height = Math.round(height * newRatio);
      }
      // And if just the height, calculate the width.
      else if (pieces[2] != '0') {
        var newHeight = parseInt(pieces[2]);
        var newRatio = newHeight / height;
        height = newHeight;
        width = Math.round(width * newRatio);
      }
    }

    // Check for a maximum dimension and scale down the width if necessary.
    // This is intended for use with Image Resize Filter.
    if (width && maxWidth && width > maxWidth) {
      var insertRatio = maxWidth / width;
      width = maxWidth;
      height = Math.round(height * insertRatio);
    }

    // Populate a insertion template.
    if (alt || title || !Drupal.settings.fileFieldInsert.templates.fileImage) {
      content = Drupal.settings.fileFieldInsert.templates.image;
    }
    else if (extension == 'png' || extension == 'jpg' || extension == 'jpeg' || extension == 'gif') {
      content = Drupal.settings.fileFieldInsert.templates.fileImage;
    }
    else {
      content = Drupal.settings.fileFieldInsert.templates.file;

      // Since the link needs a title for this tmeplate, ff description is
      // empty, use the file name.
      description = description ? description : filename;
    }

    // Poor man's escaping of invalid characters in the URL.
    url = encodeURI(url).replace(/([#$*(),?])/g, function(match) { return escape(match) });

    // Update replacements.
    content = $.trim(content).replace(/__url__/g, url).replace(/__extension__/g, extension).replace(/__width__/g, width).replace(/__height__/g, height).replace(/__description__/g, description);
    if (Drupal.settings.fileFieldInsert.templates.image) {
      content = content.replace(/__alt__/g, alt).replace(/__title__/g, title);
    }

    // Always work in normal text areas that currently have focus.
    if (fileFieldInsertTextarea && fileFieldInsertTextarea.fileFieldInsertHasFocus) {
      insertAtCursor(fileFieldInsertTextarea, content);
    }
    // WYSIWYG support, should work in all editors if available.
    else if (Drupal.wysiwyg && Drupal.wysiwyg.activeId) {
      Drupal.wysiwyg.instances[Drupal.wysiwyg.activeId].insert(content)
    }
    // Direct tinyMCE support.
    else if (typeof(tinyMCE) != 'undefined' && tinyMCE.activeEditor) {
      tinyMCE.activeEditor.execCommand('mceInsertContent', false, content);
    }
    // FCKeditor module support.
    // Requires http://drupal.org/node/445878.
    else if (typeof(FCKeditorAPI) != 'undefined' && fckActiveId) {
      FCKeditorAPI.Instances[fckActiveId].InsertHtml(content);
    }
    // Direct FCKeditor support (only body field supported).
    else if (typeof(FCKeditorAPI) != 'undefined' && FCKeditorAPI.Instances['edit-body']) {
      FCKeditorAPI.Instances['edit-body'].InsertHtml(content);
    }
    else if (fileFieldInsertTextarea) {
      insertAtCursor(fileFieldInsertTextarea, content);
    }

    return false;
  }

  function insertAtCursor(editor, content) {
    // IE support.
    if (document.selection) {
      editor.focus();
      sel = document.selection.createRange();
      sel.text = content;
    }

    // Mozilla/Firefox/Netscape 7+ support.
    else if (editor.selectionStart || editor.selectionStart == '0') {
      var startPos = editor.selectionStart;
      var endPos = editor.selectionEnd;
      editor.value = editor.value.substring(0, startPos)+ content + editor.value.substring(endPos, editor.value.length);
    }

    // Fallback, just add to the end of the content.
    else {
      editor.value += content;
    }
  }
}
