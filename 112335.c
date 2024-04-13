static bool HHVM_METHOD(Memcache, delete, const String& key,
                                          int expire /*= 0*/) {
  if (key.empty()) {
    raise_warning("Key cannot be empty");
    return false;
  }

  auto data = Native::data<MemcacheData>(this_);

  if (!hasAvailableServers(data)) {
    return false;
  }

  String serializedKey = memcache_prepare_key(key);
  memcached_return_t ret = memcached_delete(&data->m_memcache,
                                            serializedKey.c_str(),
                                            serializedKey.length(),
                                            expire);
  return (ret == MEMCACHED_SUCCESS);
}