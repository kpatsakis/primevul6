  MemcacheData(): m_memcache(), m_compress_threshold(0),
    m_min_compress_savings(0.2) {
    memcached_create(&m_memcache);

    if (MEMCACHEG(hash_strategy) == "consistent") {
      // need to hook up a global variable to set this
      memcached_behavior_set(&m_memcache, MEMCACHED_BEHAVIOR_DISTRIBUTION,
                             MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA);
    } else {
      memcached_behavior_set(&m_memcache, MEMCACHED_BEHAVIOR_DISTRIBUTION,
                             MEMCACHED_DISTRIBUTION_MODULA);
    }

    if (MEMCACHEG(hash_function) == "fnv") {
      memcached_behavior_set(&m_memcache, MEMCACHED_BEHAVIOR_HASH,
                             MEMCACHED_HASH_FNV1A_32);
    } else {
      memcached_behavior_set(&m_memcache, MEMCACHED_BEHAVIOR_HASH,
                             MEMCACHED_HASH_CRC);
    }
  };