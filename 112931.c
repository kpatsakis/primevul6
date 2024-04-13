  virtual void moduleLoad(const IniSetting::Map& ini, Hdf config) {
    HHVM_FE(hash);
    HHVM_FE(hash_algos);
    HHVM_FE(hash_file);
    HHVM_FE(hash_final);
    HHVM_FE(hash_init);
    HHVM_FE(hash_update);
    HHVM_FE(hash_copy);
    HHVM_FE(hash_equals);
    HHVM_FE(furchash_hphp_ext);
    HHVM_FE(hphp_murmurhash);
  }