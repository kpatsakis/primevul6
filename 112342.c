static bool HHVM_METHOD(Memcache, replace, const String& key,
                                           const Variant& var, int flag /*= 0*/,
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
  std::vector<char> serialized = memcache_prepare_for_storage(data, var, flag);

  memcached_return_t ret = memcached_replace(&data->m_memcache,
                                             serializedKey.c_str(),
                                             serializedKey.length(),
                                             serialized.data(),
                                             serialized.size(),
                                             expire, flag);
  return (ret == MEMCACHED_SUCCESS);
}