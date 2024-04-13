static bool HHVM_METHOD(Memcache, set, const String& key, const Variant& var,
                                       int flag /*= 0*/, int expire /*= 0*/) {
  if (key.empty()) {
    raise_warning("Key cannot be empty");
    return false;
  }

  auto data = Native::data<MemcacheData>(this_);

  if (!hasAvailableServers(data)) {
    return false;
  }

  String serializedKey = memcache_prepare_key(key);
  std::vector<char> serializedVar =
    memcache_prepare_for_storage(data, var, flag);

  memcached_return_t ret = memcached_set(&data->m_memcache,
                                         serializedKey.c_str(),
                                         serializedKey.length(),
                                         serializedVar.data(),
                                         serializedVar.size(), expire, flag);

  if (ret == MEMCACHED_SUCCESS) {
    return true;
  }

  return false;
}