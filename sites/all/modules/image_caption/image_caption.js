/*$Id: image_caption.js,v 1.2 2008/03/07 05:46:23 davidwhthomas Exp $*/
$(document).ready(function(){
  $("img.caption").each(function(i) {
    var imgwidth = $(this).width();
    var imgheight = $(this).height();
    var captiontext = $(this).attr('title');
    var alignment = $(this).attr('align');
    $(this).attr({align:""});
    $(this).wrap("<div class=\"image-caption-container\" style=\"float:" + alignment + "\"></div>");
    $(this).parent().width(imgwidth);
    $(this).parent().append("<div class=\"image-caption\">" + captiontext + "</div>");
  });
});