// $Id: README.txt,v 1.1.2.3 2009/12/19 02:38:17 roidanton Exp $

Linodef - BUEditor
------------------
This module provides functions to create buttons that uses the capabilities
of the Linodef filter.

Go to Administer -> Site building -> Modules and enable "Linodef BUEditor".

Usage
-----
Import the button given in the csv file into BUEditor. Detailed instructions
are located at http://drupal.org/node/501690.

For developers
--------------
Modules which uses taglists must include js functions the following way:
1) File name: modulename_functions.js
2) OnClick function: modulename_onclick(content); content is the output
delivered by linodef_taglists.