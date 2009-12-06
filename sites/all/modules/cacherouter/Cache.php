<?php
/**
 * $Id: Cache.php,v 1.1.2.12 2009/01/29 04:51:33 slantview Exp $
 *
 * @file Cache.php
 *   Defines the base class for cache engines
 */
class Cache {
  var $settings = array();
  var $content = array();
  var $prefix = '';
  var $name = '';
  var $lookup = '';
  var $lock = '';
  var $fast_cache = TRUE;
  var $page_cache_fastpath = TRUE;
  var $static = FALSE;
  var $lifetime = 0;
  
  function __construct($bin) {
    global $conf;
    
    $this->name = $bin;
    // Setup our prefixes so that we can prefix a particular bin, or if not set use the default prefix.
    if (!empty($conf['cacherouter'][$bin]['prefix'])) {
      $this->prefix = $conf['cacherouter'][$bin]['prefix'] .'-';
    }
    else if (!empty($conf['cacherouter']['default']['prefix'])) {
      $this->prefix = $conf['cacherouter']['default']['prefix'] .'-';
    }
    
    // This allows us to turn off fast_cache for cache_page so that we can get anonymous statistics.
    if (isset($conf['cacherouter']['default']['fast_cache'])) {
      $this->fast_cache = $conf['cacherouter']['default']['fast_cache'];
    }
    
    // This allows us to turn off static content caching for modules/bins that are already doing this.
    if (isset($conf['cacherouter'][$bin]['static'])) {
      $this->static = $conf['cacherouter'][$bin]['static'];
    }
    else if (isset($conf['cacherouter']['default']['static'])) {
      $this->static = $conf['cacherouter']['default']['static'];
    }
    
    // Setup our prefixed lookup and lock table names for shared storage.
    $this->lookup = $this->prefix . $this->name .'_lookup';
    $this->lock = $this->prefix . $this->name .'_lock';
  }
  
  function get($key) {
    if (isset($this->content[$key]) && $this->static) {
      return $this->content[$key];
    }
  }
  
  function set($key, $value) {
    if ($this->static) {
      $this->content[$key] = $value;
    }
  }
  
  function delete($key) {
    if ($this->static) {
      unset($this->content[$key]);
    }
  }
  
  function flush() {
    if ($this->static) {
      $this->content = array();
    }
  }
  
  /**
   * key()
   *   Get the full key of the item
   *
   * @param string $key
   *   The key to set.
   * @return string
   *   Returns the full key of the cache item.
   */
  function key($key) {
    return urlencode($this->prefix . $this->name .'-'. $key);
  }
}
