Variant HHVM_FUNCTION(hash_file, const String& algo, const String& filename,
                                 bool raw_output /* = false */) {
  if (filename.size() != strlen(filename.data())) {
    raise_warning(
     "hash_file() expects parameter 2 to be a valid path, string given"
    );
    return null_variant;
  }
  return php_hash_do_hash(algo, filename, true, raw_output);
}