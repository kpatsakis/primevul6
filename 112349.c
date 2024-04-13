    void moduleInit() override {
      HHVM_RC_INT(MEMCACHE_COMPRESSED, k_MEMCACHE_COMPRESSED);
      HHVM_RC_BOOL(MEMCACHE_HAVE_SESSION, true);
      HHVM_ME(Memcache, connect);
      HHVM_ME(Memcache, add);
      HHVM_ME(Memcache, set);
      HHVM_ME(Memcache, replace);
      HHVM_ME(Memcache, get);
      HHVM_ME(Memcache, delete);
      HHVM_ME(Memcache, increment);
      HHVM_ME(Memcache, decrement);
      HHVM_ME(Memcache, close);
      HHVM_ME(Memcache, getversion);
      HHVM_ME(Memcache, flush);
      HHVM_ME(Memcache, setcompressthreshold);
      HHVM_ME(Memcache, getstats);
      HHVM_ME(Memcache, getextendedstats);
      HHVM_ME(Memcache, addserver);

      Native::registerNativeDataInfo<MemcacheData>(s_MemcacheData.get());

      loadSystemlib();
    }