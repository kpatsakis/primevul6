static Variant HHVM_METHOD(Memcache, increment, const String& key,
                                                int offset /*= 1*/) {
  if (key.empty()) {
    raise_warning("Key cannot be empty");
    return false;
  }

  auto data = Native::data<MemcacheData>(this_);

  if (!hasAvailableServers(data)) {
    return false;
  }

  uint64_t value;
  String serializedKey = memcache_prepare_key(key);
  memcached_return_t ret = memcached_increment(&data->m_memcache,
                                               serializedKey.c_str(),
                                               serializedKey.length(), offset,
                                               &value);

  if (ret == MEMCACHED_SUCCESS) {
    return (int64_t)value;
  }

  return false;
}