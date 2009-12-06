function insertVideo() {
  var ed = tinyMCEPopup.editor, f = document.forms[0], nl = f.elements, v, args = {}, el;
  tinyMCEPopup.restoreSelection();

  // Fixes crash in Safari
  if (tinymce.isWebKit) {
    ed.getWin().focus();
  }

  if (nl.file.value === '') {
    ed.execCommand('mceRepaint');
    tinyMCEPopup.close();
    return;
  }
  else {
    ed.execCommand('mceInsertContent', false, '[video:'+nl.file.value+']');
    ed.undoManager.add();
    tinyMCEPopup.close();
    return;
  }
}