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
    var str = '[video:' + nl.file.value;
    if (nl.width.value !== '') {
      str += ' width:' + nl.width.value;
    }
    if (nl.height.value !== '') {
      str += ' height:' + nl.height.value;
    }
    if (nl.align.value !== 'none') {
      str += ' align:' + nl.align.value;
    }
    if (nl.autoplay.checked) {
      str += ' autoplay:' + nl.autoplay.value;
    }
    str += ']';

    ed.execCommand('mceInsertContent', false, str);
    ed.undoManager.add();
    tinyMCEPopup.close();
    return;
  }
}