static bool ini_on_update_hash_function(const std::string& value) {
  if (!strncasecmp(value.data(), "crc32", sizeof("crc32"))) {
    MEMCACHEG(hash_function) = "crc32";
  } else if (!strncasecmp(value.data(), "fnv", sizeof("fnv"))) {
    MEMCACHEG(hash_function) = "fnv";
  }
  return false;
}