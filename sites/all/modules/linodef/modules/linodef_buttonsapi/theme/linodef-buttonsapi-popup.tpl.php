<?php
// $Id: linodef-buttonsapi-popup.tpl.php,v 1.1.2.2 2008/11/30 13:00:31 roidanton Exp $
/**
 * @file
 * Theme for Linodef popup window.
 *
 * @package Linodef
 * @author Roi Danton
 */
 ?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
  <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyled Content in IE */ ?> </script>
</head>
<body>
  <div id="linodef-popup">
    <div id="header">
        <div id="closebutton"><?php print $closebutton; ?></div>
        <h2 class="title" id="page-title"><?php print ($title .': '. $element_name); ?></h2>
    </div> <!-- /header -->
    <div id="content">
        <div id="explanation"><?php print $explanation; ?></div>
        <?php if (!empty($messages)): print $messages; endif; ?>
        <?php print $content; ?>
    </div> <!-- /content -->
  </div> <!-- /linodef-popup -->
  <?php print $closure; ?>
</body>
</html>
