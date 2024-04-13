static bool ini_on_update_hash_strategy(const std::string& value) {
  if (!strncasecmp(value.data(), "standard", sizeof("standard"))) {
    MEMCACHEG(hash_strategy) = "standard";
  } else if (!strncasecmp(value.data(), "consistent", sizeof("consistent"))) {
    MEMCACHEG(hash_strategy) = "consistent";
  }
  return false;
}