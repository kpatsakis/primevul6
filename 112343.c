static Array HHVM_METHOD(Memcache, getstats,
                         const String& type /* = null_string */,
                         int slabid /* = 0 */, int limit /* = 100 */) {
  auto data = Native::data<MemcacheData>(this_);
  if (!memcached_server_count(&data->m_memcache)) {
    return Array();
  }

  char extra_args[30] = {0};

  if (slabid) {
    snprintf(extra_args, sizeof(extra_args), "%s %d %d", type.c_str(),
             slabid, limit);
  } else if (!type.empty()) {
    snprintf(extra_args, sizeof(extra_args), "%s", type.c_str());
  }

  LMCD_SERVER_POSITION_INSTANCE_TYPE instance =
    memcached_server_instance_by_position(&data->m_memcache, 0);
  const char *hostname = LMCD_SERVER_HOSTNAME(instance);
  in_port_t port = LMCD_SERVER_PORT(instance);

  memcached_stat_st stats;
  if (memcached_stat_servername(&stats, extra_args, hostname,
                                port) != MEMCACHED_SUCCESS) {
    return Array();
  }

  memcached_return_t ret;
  return memcache_build_stats(&data->m_memcache, &stats, &ret);
}