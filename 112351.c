static bool hasAvailableServers(const MemcacheData* data) {
  if (memcached_server_count(&data->m_memcache) == 0) {
    raise_warning("Memcache: No servers added to memcache connection");
    return false;
  }
  return true;
}