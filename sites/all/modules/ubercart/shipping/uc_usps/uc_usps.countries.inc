<?php
// $Id: uc_usps.countries.inc,v 1.1.2.1 2009/09/21 14:34:47 islandusurper Exp $

/**
 * @file
 * Contains the map of ISO country codes to USPS Individual Country Listings.
 */

/**
 * Return the country name formatted according to the USPS requirements.
 */
function uc_usps_country_map($code = NULL) {
  $countries = array(
    92 => 'British Virgin Islands',
    268 => 'Georgia, Republic of',
    826 => 'Great Britain and Northern Ireland',
    300 => 'Greece',
    364 => 'Iran',
    833 => 'Isle of Man (Great Britain)',
    410 => 'Korea, Republic of (South Korea)',
    428 => 'Latvia',
    440 => 'Lithuania',
    492 => 'Monaco (France)',
    104 => 'Myanmar (Burma)',
    604 => 'Peru',
    643 => 'Russia',
    688 => 'Serbia-Montenegro',
    703 => 'Slovak Republic',
    792 => 'Turkey',
    804 => 'Ukraine',
  );

  if ($code) {
    return $countries[$code];
  }

  return $countries;
}
