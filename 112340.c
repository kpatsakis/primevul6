static std::vector<char> memcache_prepare_for_storage(const MemcacheData* data,
                                                      const Variant& var,
                                                      int &flag) {
  String v;
  if (var.isString()) {
    v = var.toString();
  } else if (var.isNumeric() || var.isBoolean()) {
    flag &= ~MMC_COMPRESSED;
    v = var.toString();
  } else {
    flag |= MMC_SERIALIZED;
    v = f_serialize(var);
  }
  std::vector<char> payload;
  size_t value_len = v.length();

  if (!var.isNumeric() && !var.isBoolean() &&
    data->m_compress_threshold && value_len >= data->m_compress_threshold) {
    flag |= MMC_COMPRESSED;
  }
  if (flag & MMC_COMPRESSED) {
    size_t payload_len = compressBound(value_len);
    payload.resize(payload_len);
    if (compress((Bytef*)payload.data(), &payload_len,
                 (const Bytef*)v.data(), value_len) == Z_OK) {
      payload.resize(payload_len);
      if (payload_len >= value_len * (1 - data->m_min_compress_savings)) {
        flag &= ~MMC_COMPRESSED;
      }
    } else {
      flag &= ~MMC_COMPRESSED;
      raise_warning("could not compress value");
    }
  }
  if (!(flag & MMC_COMPRESSED)) {
    payload.resize(0);
    payload.insert(payload.end(), v.data(), v.data() + value_len);
   }
  flag |= memcache_get_flag_for_type(var);

  return payload;
}