<?php
// $Id: filefield-insert-widget.tpl.php,v 1.2 2009/04/30 23:10:04 quicksketch Exp $

/**
 * @file
 * Template file for the upload insert button.
 *
 * This button can have any appearance you like or even be a link, but must
 * include the class "filefield-insert", to which the insert JavaScript will be
 * attached.
 */
?>
<div class="filefield-insert form-item container-inline">

  <?php /* Presets are available for image fields if ImageCache is installed. */ ?>
  <?php if ($image_presets): ?>
    <div class="filefield-insert-preset">
      <label class="filefield-insert-preset"><?php print t('Image size') ?>:</label>
      <select class="filefield-insert-preset">
        <?php foreach ($image_presets as $value => $preset): ?>
          <option value="<?php print $value ?>"><?php print $preset ?></option>
        <?php endforeach; ?>
      </select>
    </div>
  <?php endif; ?>
  <input type="submit" class="form-submit filefield-insert" onclick="return false;" value="<?php print t('Send to text area'); ?>" />
</div>