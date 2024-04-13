static Variant memcache_fetch_from_storage(const char *payload,
                                           size_t payload_len,
                                           uint32_t flags) {
  Variant ret = uninit_null();

  if (flags & MMC_COMPRESSED) {
    bool done = false;
    std::vector<char> buffer;
    size_t buffer_len;
    for (int factor = 1; !done && factor <= 16; ++factor) {
      if (payload_len >=
          std::numeric_limits<unsigned long>::max() / (1 << factor)) {
        break;
      }
      buffer_len = payload_len * (1 << factor) + 1;
      buffer.resize(buffer_len);
      if (uncompress((Bytef*)buffer.data(), &buffer_len,
                     (const Bytef*)payload, (uLong)payload_len) == Z_OK) {
        done = true;
      }
    }
    if (!done) {
      raise_warning("could not uncompress value");
      return init_null();
    }
    ret = unserialize_if_serialized(buffer.data(), buffer_len, flags);
  } else {
    ret = unserialize_if_serialized(payload, payload_len, flags);
  }
  memcache_set_type_from_flag(ret, flags);

  return ret;
}