Variant HHVM_FUNCTION(hash_init, const String& algo,
                                 int64_t options /* = 0 */,
                                 const String& key /* = null_string */) {
  HashEnginePtr ops = php_hash_fetch_ops(algo);
  if (!ops) {
    raise_warning("Unknown hashing algorithm: %s", algo.data());
    return false;
  }

  if ((options & k_HASH_HMAC) && key.empty()) {
    raise_warning("HMAC requested without a key");
    return false;
  }

  void *context = malloc(ops->context_size);
  ops->hash_init(context);

  const auto hash = new HashContext(ops, context, options);
  if (options & k_HASH_HMAC) {
    hash->key = prepare_hmac_key(ops, context, key);
  }
  return Resource(hash);
}