<?php
/**
 * $Id: apc.php,v 1.1.2.14 2008/12/27 05:23:50 slantview Exp $
 *
 * @file apc.php
 *   APC engine class.
 */
class apcCache extends Cache {
  /**
   * page_fast_cache
   *   This tells CacheRouter to use page_fast_cache.
   *
   *   @return bool TRUE
   */
  function page_fast_cache() {
    return $this->fast_cache;
  }
  
  /**
   * get()
   *   Return item from cache if it is available.
   *
   * @param string $key
   *   The key to fetch.
   * @return mixed object|bool
   *   Returns either the cache object or FALSE on failure
   */
  function get($key) {
    $cache = parent::get($this->key($key));
    if (isset($cache)) {
      return $cache;
    }
    
    $cache = apc_fetch($this->key($key));
    if (is_object($cache) && $cache->serialized) {
      $cache->data = unserialize($cache->data);
    }
    parent::set($this->key($key), $cache);
    return $cache;
  }

  /**
   * set()
   *   Add item into cache.
   *
   * @param string $key
   *   The key to set.
   * @param string $value
   *   The data to store in the cache.
   * @param string $expire
   *   The time to expire in seconds.
   * @param string $headers
   *   The page headers.
   * @return bool
   *   Returns TRUE on success or FALSE on failure
   */
  function set($key, $value, $expire = CACHE_PERMANENT, $headers = NULL) {
    if ($expire == CACHE_TEMPORARY) {
      $expire = 180;
    }
    // Create new cache object.
    $cache = new stdClass;
    $cache->cid = $key;
    $cache->created = time();
    $cache->expire = $expire;
    $cache->headers = $headers;

    if (!is_string($value)) {
      $cache->serialized = TRUE;
      $cache->data = serialize($value);
    }
    else { 
      $cache->serialized = FALSE;
      $cache->data = $value;
    }

    if (!empty($key) && $this->lock()) {
      // Get lookup table to be able to keep track of bins
      $lookup = apc_fetch($this->lookup);

      // If the lookup table is empty, initialize table
      if (empty($lookup)) {
        $lookup = array();
      }

      // Set key to 1 so we can keep track of the bin
      $lookup[$this->key($key)] = 1;

      // Attempt to store full key and value
      if (!apc_store($this->key($key), $cache, $expire)) {
        unset($lookup[$this->key($key)]);
        $return = FALSE;
      }
      else {
        // Update static cache
        parent::set($this->key($key), $cache);
        $return = TRUE;
      }
      
      // Resave the lookup table (even on failure)
      apc_store($this->lookup, $lookup);
      
      // Remove lock.
      $this->unlock();
    }

    return $return;
  }
  
  /**
   * delete()
   *   Remove item from cache.
   *
   * @param string $key
   *   The key to set.
   * @return mixed object|bool
   *   Returns either the cache object or FALSE on failure
   */
  function delete($key) {
    // Remove from static array cache.
    parent::delete($this->key($key));

    if (substr($key, strlen($key) - 1, 1) == '*') {
      $key = $this->key(substr($key, 0, strlen($key) - 1));
      $lookup = apc_fetch($this->lookup);
      if (!empty($lookup)) {
        foreach ($lookup as $k => $v) {
          if (substr($k, 0, strlen($key)) == $key) {
            apc_delete($k);
            unset($lookup[$k]);
          }
        }
      }
      if ($this->lock()) {
        apc_store($this->lookup, $lookup);
        $this->unlock();
      }
    }
    else {
      if (!empty($key)) {
        if (!apc_delete($this->key($key))) {
          $return = FALSE;
        }
        else {
          $return = TRUE;
        }
      }
    }
  }

  /**
   * flush()
   *   Flush the entire cache.
   *
   * @param none
   * @return mixed bool
   *   Returns TRUE
   */
  function flush() {
    parent::flush();
    if ($this->lock()) {
      // Get lookup table to be able to keep track of bins
      $lookup = apc_fetch($this->lookup);
    
      // If the lookup table is empty, remove lock and return
      if (empty($lookup)) {
        $this->unlock();
        return TRUE;
      }

      // Cycle through keys and remove each entry from the cache
      foreach ($lookup as $k => $v) {
        if (apc_delete($k)) {
          unset($lookup[$k]);
        }
      }

      // Resave the lookup table (even on failure)
      apc_store($this->lookup, $lookup);

      // Remove lock
      $this->unlock();
    }
    
    return TRUE;
  }

  /**
   * lock()
   *   lock the cache from other writes.
   *
   * @param none
   * @return string
   *   Returns TRUE on success, FALSE on failure
   */
  function lock() {
    if (function_exists('apc_add')) {
      // Lock once by trying to add lock file, if we can't get the lock, we will loop
      // for 3 seconds attempting to get lock.  If we still can't get it at that point,
      // then we give up and return FALSE.
      if (apc_add($this->lock, TRUE) === FALSE) {
        $time = time();
        while (apc_add($this->lock, TRUE) === FALSE) {
          if (time() - $time >= 3) {
            return FALSE;
          }
        }
      }
    }
    else {
      // For old versions of apc (before 3.0.13)
    	$time = time();
    	while (apc_fetch($this->lock)) {
        if (time() - $time >= 3) {
          return FALSE;
        }
    	}
    	apc_store($this->lock, TRUE);
    }
    return TRUE;
  }
  
  /**
   * unlock()
   *   lock the cache from other writes.
   *
   * @param none
   * @return bool
   *   Returns TRUE on success, FALSE on failure
   */
  function unlock() {
    return apc_delete($this->lock);
  }
  
  function stats() {
    $apc_stats = apc_cache_info('user', TRUE);
    $apc_stats['uptime'] = time() - $apc_stats['start_time'];
    $stats = array(
      'uptime' => $apc_stats['uptime'],
      'bytes_used' => $apc_stats['mem_size'],
      'bytes_total' => ini_get('apc.shm_size') * 1024 * 1024,
      'gets' => $apc_stats['num_hits'],
      'sets' => $apc_stats['num_inserts'],
      'hits' => $apc_stats['num_hits'] + $apc_stats['num_misses'],
      'misses' => $apc_stats['num_misses'],
      'req_rate' => (($apc_stats['num_hits'] + $apc_stats['num_misses']) / $apc_stats['uptime']),
      'hit_rate' => ($apc_stats['num_hits'] / $apc_stats['uptime']),
      'miss_rate' => ($apc_stats['num_misses'] / $apc_stats['uptime']),
      'set_rate' => ($apc_stats['num_inserts'] / $apc_stats['uptime']),
    );
    return $stats;
  }
}