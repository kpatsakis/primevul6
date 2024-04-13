    void threadInit() override {
      assertx(!s_memcache_globals);
      s_memcache_globals = new MEMCACHEGlobals;
      IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                       "memcache.hash_strategy", "standard",
                       IniSetting::SetAndGet<std::string>(
                         ini_on_update_hash_strategy,
                         nullptr
                       ),
                       &MEMCACHEG(hash_strategy));
      IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                       "memcache.hash_function", "crc32",
                       IniSetting::SetAndGet<std::string>(
                         ini_on_update_hash_function,
                         nullptr
                       ),
                       &MEMCACHEG(hash_function));
    }