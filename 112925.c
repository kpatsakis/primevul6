static void finalize_hmac_key(char *K, HashEnginePtr ops, void *context,
                              char *digest) {
  /* Convert K to opad -- 0x6A = 0x36 ^ 0x5C */
  for (int i = 0; i < ops->block_size; i++) {
    K[i] ^= 0x6A;
  }

  /* Feed this result into the outter hash */
  ops->hash_init(context);
  ops->hash_update(context, (unsigned char *)K, ops->block_size);
  ops->hash_update(context, (unsigned char *)digest, ops->digest_size);
  ops->hash_final((unsigned char *)digest, context);

  /* Zero the key */
  memset(K, 0, ops->block_size);
  free(K);
}