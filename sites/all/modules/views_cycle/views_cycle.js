// $Id: views_cycle.js,v 1.2 2009/03/20 21:55:33 crell Exp $

Drupal.behaviors.viewsCycle = function(context) {

  var settings = Drupal.settings.views_cycle;

  $('.views-cycle-container:not(.viewsCycle-processed)', context).addClass('viewsCycle-processed').each(function () {

    var cycler = $(this);
    var id = cycler.attr('id');
    var config = settings[id];
    var tallest = 0;
    var verticalPadding = settings.verticalPadding || 5;

    // If we have a pager of some kind, create our pager placeholder.
    if (config.params.pager) {
      cycler.before('<ul class="view-cycle-pager" id="' + id + '-nav">');
    }

    // Find the tallest item and set the height to that item's height + padding.
    cycler.children('li').each(function () {
      var li = $(this);
      if (li.height() > tallest) {
        tallest = li.height();
      }
    });
    cycler.height(tallest + verticalPadding);

		// Adding height to views-cycle parent div so that ie7 plays nice.
		cycler.parents('.views-cycle').height(tallest + verticalPadding);

    // If thumbnails are used, this will always be how they're created.
    // config.thumbnails is pre-generated in PHP so we can use Drupal's theming system.
    function makeAnchors(idx, slide) {
      return '<li><a href="#">' + config.thumbnails[idx] + "</a></li>\n";
    }

    // We can't set the function from PHP because we can't specify a datatype of
    // "function" from PHP.  So we simply flag it to use the function above.
    if (config.use_pager_callback) {
      config.params.pagerAnchorBuilder = makeAnchors;
    }

    // Fire away!
    cycler.cycle(config.params);
  });
};
