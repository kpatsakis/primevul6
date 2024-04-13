static char *prepare_hmac_key(HashEnginePtr ops, void *context,
                              const String& key) {
  char *K = (char*)malloc(ops->block_size);
  memset(K, 0, ops->block_size);
  if (key.size() > ops->block_size) {
    /* Reduce the key first */
    ops->hash_update(context, (unsigned char *)key.data(), key.size());
    ops->hash_final((unsigned char *)K, context);
    /* Make the context ready to start over */
    ops->hash_init(context);
  } else {
    memcpy(K, key.data(), key.size());
  }

  /* XOR ipad */
  for (int i = 0; i < ops->block_size; i++) {
    K[i] ^= 0x36;
  }
  ops->hash_update(context, (unsigned char *)K, ops->block_size);
  return K;
}