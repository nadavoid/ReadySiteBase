(function() {
  tinymce.create('tinymce.plugins.VideoFilterPlugin', {
    init : function(ed, url) {
      // Register commands
      ed.addCommand('mceVideoFilter', function() {
        ed.windowManager.open({
          file : Drupal.settings.basePath + 'video_filter/load',
          width : 480,
          height : 480,
          inline : 1,
          scrollbars : 1
        });
      });

      // Register buttons
      ed.addButton('videofilter', {
        title : 'Video Filter',
        cmd : 'mceVideoFilter'
      });
    },

    getInfo : function() {
      return {
        longname : 'Video Filter',
        author : 'Video Filter',
        authorurl : 'http://drupal.org/project/video_filter',
        infourl : 'http://drupal.org/project/video_filter',
        version : tinymce.majorVersion + "." + tinymce.minorVersion
      };
    }
  });

  // Register plugin
  tinymce.PluginManager.add('videofilter', tinymce.plugins.VideoFilterPlugin);
})();