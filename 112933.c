Variant HHVM_FUNCTION(hash, const String& algo, const String& data,
                            bool raw_output /* = false */) {
  return php_hash_do_hash(algo, data, false, raw_output);
}