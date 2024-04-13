int64_t HHVM_FUNCTION(furchash_hphp_ext, const String& key,
                                         int64_t len, int64_t nPart) {
  len = std::max<int64_t>(std::min<int64_t>(len, key.size()), 0);
  return furc_hash(key.data(), len, nPart);
}