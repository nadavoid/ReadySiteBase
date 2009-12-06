<?php
// $Id: views_cycle.theme.inc,v 1.1 2009/03/20 21:47:24 crell Exp $

/**
 * Preprocess function to build the cycle style plugin.
 */
function template_preprocess_views_cycle(&$vars, $hook) {

  // Some nice aliases to make the code easier to read.
  $view     = $vars['view'];
  $options  = $view->style_plugin->options;
  $handler  = $view->style_plugin;
  $fields   = &$view->field;

  $rows     = $vars['rows'];

  // Generate a unique ID for this cycle instance that we can pass to JS.
  $vars['cycle_id'] = 'views-cycle-'. $view->name .'-'. $view->style_plugin->display->id;

  // The $settings array is passed forward to Javascript to inform the code how
  // to build the cycle slideshow.  We'll convert this to full Drupal.settings
  // format below.
  $settings = array(
    'params' => $options['params'],
  );

  // Javascript, or at least the cycle plugin, gets very cranky if you pass it
  // a string version of a number.  We therefore have to force all numeric
  // values to actually be numeric data types so that they are passed as such
  // to Javascript.  Fortunately we know that all of the numeric values that
  // cycle takes are integers.
  foreach ($settings['params'] as $key => $value) {
    if (is_numeric($value)) {
      $settings['params'][$key] = (int)$value;
    }
  }

  // If there are thumbnails, we need to build that list as well as fully rendered
  // HTML and pass it to Javascript.  Technically the JS needs to "generate" the
  // HTML for the thumbnails for the cycle plugin to work, but we'll pre-theme
  // that server side and then just look it up on demand in the browser.
  if ($options['thumbnail_field']) {
    foreach ($view->result as $num => $row) {
      $settings['thumbnails'][$num] = $view->field[$options['thumbnail_field']]->theme($row);
    }
    $settings['params']['pager']      = '#'. $vars['cycle_id'] .'-nav';
    $settings['use_pager_callback']   = 1;
    $settings['params']['pagerEvent'] = $options['pager']['event'];
  }

  // Enqueue any stylesheets set for the skin on this view are added.
  drupal_add_css(drupal_get_path('module', 'views_cycle') .'/views_cycle.css');
  $skin_path = drupal_get_path('module', $options['skin_info']['module']);
  if ($options['skin_info']['path']) {
    $skin_path .= '/'. $options['skin_info']['path'];
  }
  foreach ($options['skin_info']['stylesheets'] as $stylesheet) {
    drupal_add_css($skin_path .'/'. $stylesheet);
  }

  // If this is an AHAH request, we have to forward the updated thumbnail
  // thumbnail information manually.  We assume that jQuery, the cycle plugin,
  // and so forth are already on the page so we don't add them again.
  if (isset($_GET['pager_element'])) {
    $thumbnails = drupal_to_js($settings['thumbnails']);

    // I'm pretty sure that Drupal.settings will not exist at all if this is called
    // via an AHAH request, but just to be on the safe side we'll go through the
    // process of checking each level.  Why can't Javascript just be like PHP
    // and lazy create all down the line?  *sigh*
    $vars['js_settings'] = <<<JS
<script type="text/javascript">
  <!--//--><![CDATA[//><!--
    if (!Drupal) {
      var Drupal = {};
    }
    if (!Drupal.settings) {
      Drupal.settings = {};
    }
    if (!Drupal.settings.views_cycle) {
      Drupal.settings.views_cycle = {};
    }
    if (!Drupal.settings.views_cycle['{$vars['cycle_id']}']) {
      Drupal.settings.views_cycle['{$vars['cycle_id']}'] = {};
    }
    Drupal.settings.views_cycle['{$vars['cycle_id']}'].thumbnails = {$thumbnails};
  //--><!]]>
</script>

JS;
  }
  else {
    // For a normal page request, we need to pass data through the normal Drupal
    // Javascript handling routines.

    // Set up the necessary JS.
    drupal_add_js(drupal_get_path('module', 'views_cycle') .'/jquery.cycle.js', 'module', 'footer');
    drupal_add_js(drupal_get_path('module', 'views_cycle') .'/views_cycle.js', 'module', 'footer');

    // Key the settings for each instance of the cycle plugin by its cycle id.
    drupal_add_js(array('views_cycle' => array($vars['cycle_id'] => $settings)), 'setting', 'footer');
  }
}

