; $Id: README.txt,v 1.2.2.2 2010/07/20 19:16:35 crell Exp $

ABOUT

This module provides a Views style plugin for using the jQuery cycle plugin.  It 
is primarily useful for creating image slideshows, but should be usable on any
content.

This module relies on the jQuery cycle plugin, which must be downloaded 
separately from:

http://www.malsup.com/jquery/cycle/

It is loaded via the Libraries module, which is a requirement.


REQUIREMENTS

- PHP 5.2
- Drupal 6.x
- Views 2
- Libraries API
- The jQuery cycle plugin: http://www.malsup.com/jquery/cycle/


INSTALLATION INSTRUCTIONS

1) Download and enable the Libraries API module: http://drupal.org/project/libraries

2) Download the jQuery cycle plugin: http://www.malsup.com/jquery/cycle/.  You 
want the "full" package, not the Lite version.

3) Unzip the jQuery cycle plugin download.  It will create a directory called 
"jquery.cycle".  Place that directory in sites/all/libraries.  (Other
library locations such as sites/default/libraries work too, but sites/all is 
recommended.)

4) Enable this module.


IMPORTANT NOTE

Because this module adds dynamic Javascript configuration as part of its theme
preprocess routine, it is not compatible with Views output caching.  It should
be fine with query caching but output caching will break.  Sorry, I don't know
of a way around that.  If you can think of one, please file an issue. 


AUTHOR AND CREDIT

Larry Garfield
http://www.palantir.net/

This module was initially developed by Palantir.net.
