  ~MemcacheData() {
    memcached_free(&m_memcache);
  };