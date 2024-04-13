    void threadShutdown() override {
      delete s_memcache_globals;
      s_memcache_globals = nullptr;
    }