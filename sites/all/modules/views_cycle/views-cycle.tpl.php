<?php // $Id: views-cycle.tpl.php,v 1.1 2009/03/20 21:47:24 crell Exp $ ?>
<?php echo $js_settings; ?>
<div class="views-cycle item-list">
  <?php if (!empty($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <<?php print $options['type']; ?> class="views-cycle-container" id="<?php echo $cycle_id; ?>">
    <?php foreach ($rows as $row): ?>
      <li><?php print $row; ?></li>
    <?php endforeach; ?>
  </<?php print $options['type']; ?>>
</div>
