static Variant HHVM_METHOD(Memcache, getversion) {
  auto data = Native::data<MemcacheData>(this_);
  int server_count = memcached_server_count(&data->m_memcache);
  char version[16];
  int version_len = 0;

  if (memcached_version(&data->m_memcache) != MEMCACHED_SUCCESS) {
    return false;
  }

  for (int x = 0; x < server_count; x++) {
    LMCD_SERVER_POSITION_INSTANCE_TYPE instance =
      memcached_server_instance_by_position(&data->m_memcache, x);
    uint8_t majorVersion = LMCD_SERVER_MAJOR_VERSION(instance);
    uint8_t minorVersion = LMCD_SERVER_MINOR_VERSION(instance);
    uint8_t microVersion = LMCD_SERVER_MICRO_VERSION(instance);

    if (!majorVersion) {
      continue;
    }

    version_len = snprintf(version, sizeof(version),
        "%" PRIu8 ".%" PRIu8 ".%" PRIu8,
        majorVersion, minorVersion, microVersion);
    return String(version, version_len, CopyString);
  }

  return false;
}