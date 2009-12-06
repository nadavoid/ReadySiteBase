<?php
// $Id: filefield-insert-image.tpl.php,v 1.2 2009/05/10 19:50:58 quicksketch Exp $

/**
 * Template for images that are inserted into the editor. This applies to all
 * JPG, GIF, and PNG images for both ImageField and FileField.
 *
 * Available variables:
 *  - field: The CCK field array.
 *  - $url: The path to the uploaded file.
 *  - $description: The current description of the file.
 *  - $alt: The current description of the file.
 *  - $title: The current description of the file.
 *  - $extension: The file extension.
 * 
 * Note that these variables are only placeholders, they will be filled with
 * their real values by JavaScript.
 */
?>
<?php if ($field['widget']['module'] == 'imagefield'): // Use the ALT and Title fields if available. ?>
  <img src="<?php print $url ?>" alt="<?php print $alt ?>" title="<?php print $title ?>" width="<?php print $width ?>" height="<?php print $height ?>" />
<?php else: // For FileField, use the description for both fields. ?>
  <img src="<?php print $url ?>" alt="<?php print $description ?>" title="<?php print $description ?>" width="<?php print $width ?>" height="<?php print $height ?>" />
<?php endif; ?>