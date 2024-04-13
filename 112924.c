static Variant php_hash_do_hash(const String& algo, const String& data,
                                bool isfilename,
                                bool raw_output) {
  HashEnginePtr ops = php_hash_fetch_ops(algo);
  if (!ops) {
    raise_warning("Unknown hashing algorithm: %s", algo.data());
    return false;
  }
  Variant f;
  if (isfilename) {
    f = f_fopen(data, "rb");
    if (same(f, false)) {
      return false;
    }
  }

  void *context = malloc(ops->context_size);
  ops->hash_init(context);

  if (isfilename) {
    for (Variant chunk = f_fread(f.toResource(), 1024);
         !is_empty_string(chunk);
         chunk = f_fread(f.toResource(), 1024)) {
      String schunk = chunk.toString();
      ops->hash_update(context, (unsigned char *)schunk.data(), schunk.size());
    }
  } else {
    ops->hash_update(context, (unsigned char *)data.data(), data.size());
  }

  String raw = String(ops->digest_size, ReserveString);
  char *digest = raw.bufferSlice().ptr;
  ops->hash_final((unsigned char *)digest, context);
  free(context);

  raw.setSize(ops->digest_size);
  if (raw_output) {
    return raw;
  }
  return f_bin2hex(raw);
}