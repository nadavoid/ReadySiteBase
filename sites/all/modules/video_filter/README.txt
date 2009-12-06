$Id: README.txt,v 1.2.2.5 2009/10/22 19:40:44 blackdog Exp $

This is a highly flexible and easy extendable filter module to embed any type of video in your site using a simple tag. Other modules can add video sites/formats (called codecs) using an easy plug-in architecture.

========= Installation =========

Enable the module on the modules page.

Go to admin/settings/filters and configure the input format(s) that should be allowed to use this filter. Check the box to enable Video Filter and save. Some simple settings are available if you configure the input format. There you can change the default size and auto play settings.

To enable WYSIWYG support, go to the WYSIWYG settings for each input format and enable the Video Filter button.

========= Usage =========

Single video: [video:url]
This will output the video using the default settings.

Random video from multiple URL's: [video:url,url]
This will output one of the specified videos each time.

You can also set some parameters in the call:

[video:url width:X height:Y align:left/right autoplay:1/0]
This will override the default settings for this video.

========= Developers =========

This module calls hook_codec_info(), so you can add your own codecs.

Example:

function MODULE_codec_info() {
  $codecs = array();
  // You can offer multiple video formats in one module.
  $codecs['youtube'] = array(
    // Will be used some day in user information.
    'name' => t('YouTube'),
    
    // Special instructions for end users. Optional.
    'instructions' => t('Any special instructions that users need to know about to get this codec working correctly.'),

    // The callback that will output the right embed code.
    'callback' => 'MODULE_youtube',

    // Regexp can be an array. $video['codec']['delta'] will be set to the key.
    'regexp' => '/youtube\.com\/watch\?v=([a-z0-9]+)/i',

    // Ratio for resizing within user-given width and height (ratio = width / height)
    'ratio' => 425 / 355,
  );
  return $codecs;
}

And this will be your callback function:

function MODULE_youtube($video) {
  // $video contains the video URL in source, the codec (as above) and also [code][matches] with the result of the regexp and [codec][delta] with the key of the matched regexp.
  $video['source'] = 'http://www.youtube.com/v/'.$video['codec']['matches'][1].($video['autoplay'] ? '&autoplay=1' : '');
  
  // Outputs a general <object...> for embedding flash players. Needs width, height, source and optionally align (left or right) and params (a list of <param...> attributes)
  return video_filter_flash($video);
}

========= Troubleshooting =========

If videos don't show up, try disabling any browser plugins, such as AdBlock.