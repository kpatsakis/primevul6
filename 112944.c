int64_t HHVM_FUNCTION(hphp_murmurhash, const String& key,
                                       int64_t len, int64_t seed) {
  len = std::max<int64_t>(std::min<int64_t>(len, key.size()), 0);
  return murmur_hash_64A(key.data(), len, seed);
}