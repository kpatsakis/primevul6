Variant HHVM_FUNCTION(hash_final, const Resource& context,
                                 bool raw_output /* = false */) {
  HashContext *hash = context.getTyped<HashContext>();

  if (hash->context == nullptr) {
    raise_warning(
      "hash_final(): supplied resource is not a valid Hash Context resource"
    );
    return false;
  }

  String raw = String(hash->ops->digest_size, ReserveString);
  char *digest = raw.bufferSlice().ptr;
  hash->ops->hash_final((unsigned char *)digest, hash->context);
  if (hash->options & k_HASH_HMAC) {
    finalize_hmac_key(hash->key, hash->ops, hash->context, digest);
    hash->key = NULL;
  }
  free(hash->context);
  hash->context = NULL;

  raw.setSize(hash->ops->digest_size);
  if (raw_output) {
    return raw;
  }
  return f_bin2hex(raw);
}