static Variant unserialize_if_serialized(const char *payload,
                                         size_t payload_len,
                                         uint32_t flags) {
  Variant ret = uninit_null();
  if (flags & MMC_SERIALIZED) {
    ret = unserialize_from_buffer(
      payload,
      payload_len,
      VariableUnserializer::Type::Serialize
    );
  } else {
    if (payload_len == 0) {
      ret = empty_string();
    } else {
      ret = String(payload, payload_len, CopyString);
    }
  }
  return ret;
 }