HHVM_METHOD(Memcache, get, const Variant& key, VRefParam /*flags*/ /*= null*/) {
  auto data = Native::data<MemcacheData>(this_);

  if (!hasAvailableServers(data)) {
    return false;
  }

  if (key.isArray()) {
    std::vector<const char *> real_keys;
    std::vector<size_t> key_len;
    Array keyArr = key.toArray();

    real_keys.reserve(keyArr.size());
    key_len.reserve(keyArr.size());

    for (ArrayIter iter(keyArr); iter; ++iter) {
      auto key = iter.second().toString();
      String serializedKey = memcache_prepare_key(key);
      char *k = new char[serializedKey.length()+1];
      memcpy(k, serializedKey.c_str(), serializedKey.length() + 1);
      real_keys.push_back(k);
      key_len.push_back(serializedKey.length());
    }

    if (!real_keys.empty()) {
      const char *payload = nullptr;
      size_t payload_len = 0;
      uint32_t flags = 0;
      const char *res_key = nullptr;
      size_t res_key_len = 0;

      memcached_result_st result;

      memcached_return_t ret = memcached_mget(&data->m_memcache, &real_keys[0],
                                              &key_len[0], real_keys.size());
      memcached_result_create(&data->m_memcache, &result);

      // To mimic PHP5 should return empty array at failure.
      Array return_val = Array::Create();

      while ((memcached_fetch_result(&data->m_memcache, &result, &ret))
             != nullptr) {
        if (ret != MEMCACHED_SUCCESS) {
          // should probably notify about errors
          continue;
        }

        payload     = memcached_result_value(&result);
        payload_len = memcached_result_length(&result);
        flags       = memcached_result_flags(&result);
        res_key     = memcached_result_key_value(&result);
        res_key_len = memcached_result_key_length(&result);

        return_val.set(String(res_key, res_key_len, CopyString),
                       memcache_fetch_from_storage(payload,
                                                   payload_len, flags));
      }
      memcached_result_free(&result);
      for ( size_t i = 0 ; i < real_keys.size() ; i++ ) {
        delete [] real_keys[i];
      }

      return return_val;
    }
  } else {
    char *payload = nullptr;
    size_t payload_len = 0;
    uint32_t flags = 0;

    memcached_return_t ret;
    String serializedKey = memcache_prepare_key(key.toString());

    if (serializedKey.length() == 0) {
      return false;
    }

    payload = memcached_get(&data->m_memcache, serializedKey.c_str(),
                            serializedKey.length(), &payload_len, &flags, &ret);

    /* This is for historical reasons from libmemcached*/
    if (ret == MEMCACHED_END) {
      ret = MEMCACHED_NOTFOUND;
    }

    if (ret == MEMCACHED_NOTFOUND) {
      return false;
    }

    if (ret != MEMCACHED_SUCCESS) {
      return false;
    }

    Variant retval = memcache_fetch_from_storage(payload, payload_len, flags);
    free(payload);

    return retval;
  }
  return false;
}