<?php // $Id: views-cycle.tpl.php,v 1.1.2.4 2010/07/20 18:26:37 crell Exp $ ?>
<?php echo $js_settings; ?>
<div class="views-cycle item-list">
  <?php if (!empty($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <<?php print $options['type']; ?> class="views-cycle-container js-hide" id="<?php echo $cycle_id; ?>">
    <?php foreach ($rows as $id => $row): ?>
      <li class="<?php print $classes[$id]; ?>"><?php print $row; ?></li>
    <?php endforeach; ?>
  </<?php print $options['type']; ?>>
<?php if (!empty($thumbs_data)) : ?>
  <ul id='<?php print $cycle_id; ?>-thumb-data' style='display: none;'><?php print $thumbs_data; ?></ul>
<?php endif; ?>
</div>
