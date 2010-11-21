<?php
// $Id: linodef-filter-tips.inc,v 1.1.2.6 2009/07/06 12:02:02 roidanton Exp $

/**
 * @file
 * The help text for Linodef, used in main module file.
 */

/**
 * Help text.
 *
 * @return
 *   Text with html markup.
 */
function _linodef_filter_tips() {
	global $base_url;
	$output  = '<a name="linodef"><h2><strong>'. t('Linodef') .'</strong></h2></a>';
	$output .= '<p>'. t("%Linodef is a filter for internal links. It uses a simple but unique markup so %Linodef doesn't interfere with other filter modules.", array('%Linodef' => 'Linodef')) .'<br />';
	$output .= t("The ID of a node or of a taxonomy term and optionally a fieldname are the solely elements stored in your text. One advantage of this method in comparison to similar filter modules is the fact that the links don't break if you change the node title or term name. Furthermore, if you change the title of the node or the value of a field or the name of a term then those are automatically updated in your text, too.") .'</p>';

	/**
	 * Embed elements.
	 */
	$output .= '<h3 style="text-decoration: underline;">'. t('Embed elements') .'</h3>';
	$output .= '<p>'. t("To embed elements into your text, prepend the element ID with the hash key # and surround them by brackets []: [#ID]. That way a node title is inserted and linked. To embed the content of a field, add the field name: [#ID:field_name]. Moreover it is possible to add a field value number in the case you have a field with multiple values and want to use a different value than the first: [#ID:field_name:number].") .'</p>'. theme('advanced_help_topic', 'linodef', 'fieldtypes');
	$output .= '<p>'. t('The syntax is as following:') .'</p>';
	// Syntax table.
	$tips = array(
		array('name' => 'nid',                  'desc' => t('Embed node title and link to the node.'),  'type' => '[#ID]', 'get' => '<a href="'. $base_url .'/node/NodeID">'. t('node title') .'</a>'),
		array('name' => 'nid_field',            'desc' => t('Embed field value and link to the node.'), 'type' => '[#ID:field_name]', 'get' => '<a href="'. $base_url .'/node/NodeID">'. t('field value') .'</a>'),
		array('name' => 'nid_field_multiple',   'desc' => t('Embed field value and link to the node.'), 'type' => '[#ID:field_name:field_value_number]', 'get' => '<a href="'. $base_url .'/node/NodeID">'. t('certain field value of a multiple values field') .'</a>'),
	);
	$output .= _linodef_filter_tips_maketable($tips);
	// Example
	$output .= '<h4>'. t('Example for nodes and fields') .':</h4>';
	$output .= t('We have a node with id=!id and a title %title. It contains a multiple value field called !firstnames. The first value is %value1, second is %value2. Note that the field value numbers start at 0, so first value has number &quot;0&quot;, second has number &quot;1&quot; etc. If the node has a path alias then the link contains this instead of the default drupal path (the examples are limited to the default paths).', array('!id' => '<strong>8</strong>', '!firstnames' => '<em>firstnames</em>', '%title' => 'Perry Rhodan', '%value1' => 'Gucky', '%value2' => 'Atlan'));
	// Example table
	$tips = array(
		array('name' => 'nid',                      'type' => '[#8]', 'get' => '<a href="'. $base_url .'/node/8">Perry Rhodan</a>'),
		array('name' => 'nid_field',                'type' => '[#8:field_firstnames]', 'get' => '<a href="'. $base_url .'/node/8">Gucky</a>'),
		array('name' => 'nid_field_multiple',       'type' => '[#8:field_firstnames:0]', 'get' => '<a href="'. $base_url .'/node/8">Gucky</a>'),
		array('name' => 'nid_field_multiple',       'type' => '[#8:field_firstnames:1]', 'get' => '<a href="'. $base_url .'/node/8">Atlan</a>'),
	);
	$output .= _linodef_filter_tips_maketable($tips);

	/**
	 * Embed elements with options.
	 */
	$output .= theme('advanced_help_topic', 'linodef', 'options') .'<h3 style="text-decoration: underline;">'. t('Alter embedding by tag options') .'</h3>';
	$output .= '<p>'. t('Options set the ID type (embedded element type) and the embed method. They are entered right after the ID or the field settings and are separated by a comma. The number of options a tag can have is not limited.') .'</p>';
	// Syntax table.
	$tips = array(
		array('name' => 'option',              'desc' => t('Embed an element with options.'), 'type' => '[#ID,option1,option2="optionvalue",...]', 'get' => t('Depends on the options used.')),
		array('name' => 'option',              'desc' => t('Embed a field value with options.'), 'type' => '[#ID:field_name,option1,option2="optionvalue",...]', 'get' => t('Depends on the options used.')),
		array('name' => 'option',              'desc' => t('Embed a certain field value with options.'), 'type' => '[#ID:field_name:field_value_number,option1,option2="optionvalue",...]', 'get' => t('Depends on the options used.')),
	);
	$output .= _linodef_filter_tips_maketable($tips);
	// Example
	$output .= '<h4>'. t('Example for element (ID) type and modification options') .':</h4>';
	$output .= '<p>'. t('Here we use the modification "nolink" to have an output without a link to the element and the ID type option "tid" to embed a taxonomy term instead of a node. The taxonomy term has the id=!id and the name %termname. If the term has a path alias then the link contains this instead of the default drupal path (the example is limited to the default path). Furthermore we have a node (optional option "nid") with id=!id and the title %nodetitle.', array('!id' => '<strong>5</strong>', '%termname' => 'Jupiter', '%nodetitle' => 'Kallisto'));
	$tips = array(
		array('name' => 'option_tid',              'type' => '[#5,tid]', 'get' => '<a href="'. $base_url .'/taxonomy/term/5">Jupiter</a>'),
		array('name' => 'option_tidnl',            'type' => '[#5,tid,nolink]', 'get' => 'Jupiter'),
		array('name' => 'option_nidnl',            'type' => '[#5,nid,nolink]', 'get' => 'Kallisto'),
		array('name' => 'option_nl',               'type' => '[#5,nolink]', 'get' => 'Kallisto'),
	);
	$output .= _linodef_filter_tips_maketable($tips);

	/**
	 * Comment usage.
	 */
	$output .= theme('advanced_help_topic', 'linodef', 'comments') .'<h3 style="text-decoration: underline;">'. t('Use comments to flag your tags for editors') .'</h3>';
	$output .= '<p>'. t('Comments act as an indicator for Linodef tags, to describe your tag for yourself or other editors. A comment can be inserted inside the tag prepended with a space and it may consist of all UTF-8 characters except the closing bracket ].') .'</p>';
	$output .= '<p>'. t('Comments are visible only during editing and never appear by viewing a text. The nature of comments is that they must not modified automatically and therefore will not change unless you change it. This is important to understand since the content the tag displays can change automatically and so the comment may be outdated. So try to use a common description.') .'</p>';
	// Syntax table.
	$tips = array(
		array('name' => 'comments',             'desc' => t('Use comments to give information about the tag during editing.'),  'type' => '[#ID comment]', 'get' => t('node title')),
		array('name' => 'comments',             'desc' => t('Comment a field content.'),                                        'type' => '[#ID:field_name:field_value_number comment]', 'get' => t('node title')),
		array('name' => 'options',              'desc' => t('Comments work together with options.'),                            'type' => '[#ID:field_name:field_value_number,option1,option2="optionvalue",... comment]', 'get' => t('Depends on the options used.')),
	);
	$output .= _linodef_filter_tips_maketable($tips);

	/**
	 * Link nodes.
	 */
	$output .= '<h3 style="text-decoration: underline;">'. t('Create links with own linktext') .'</h3>';
	// Syntax
	$output .= '<p>'. t('Use the option "linktext" to create links to elements using a custom text: [#ID,linktext="your Text"]') .'</p>';
	// Example
	$output .= '<h4>'. t('Example') .':</h4>';
	// Example table
	$tips = array(
		array('name' => 'option_linktext',      'type' => t('[#8,linktext="your Text"]'), 'get' => '<a href="'. $base_url .'/node/8">'. t('your Text') .'</a>'),
	);
	$output .= _linodef_filter_tips_maketable($tips);

	// Way 3: Link & embed terms.

	// Syntax

	// Example

	return $output;
}

/**
 * Create formatted table of an array using similar layout as core filter module.
 *
 * If $tips has no desc, only two columns are used.
 *
 * @param array $tips
 *   Array with type, get (and desc)
 * @return
 *   Returns the table with appropriate html markup.
 */
function _linodef_filter_tips_maketable($tips) {
	for ($i = 0; $i < count($tips); $i++) {
		if ($tips[$i]['desc']) {
				$rows[] = array(
					array('data' => $tips[$i]['desc'], 'class' => 'description'),
					array('data' => '<code>'. $tips[$i]['type'] .'</code>', 'class' => 'type'),
					array('data' => '<code>'. $tips[$i]['get'] .'</code>', 'class' => 'get')
				);
		}
		else {
				$rows[] = array(
					array('data' => '<code>'. $tips[$i]['type'] .'</code>', 'class' => 'type'),
					array('data' => '<code>'. $tips[$i]['get'] .'</code>', 'class' => 'get')
				);
		}
	}
	if ($tips[0]['desc']) {
		$header = array(t('Tag Description'), t('You Type'), t('You Get'));
	}
	else {
		$header = array(t('You Type'), t('You Get'));
	}
	return theme('table', $header, $rows);
}