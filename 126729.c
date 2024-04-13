void mg_hmac_sha1(const unsigned char *key, size_t keylen,
                  const unsigned char *data, size_t datalen,
                  unsigned char out[20]) {
  mg_sha1_ctx ctx;
  unsigned char buf1[64], buf2[64], tmp_key[20], i;

  if (keylen > sizeof(buf1)) {
    mg_sha1_init(&ctx);
    mg_sha1_update(&ctx, key, keylen);
    mg_sha1_final(tmp_key, &ctx);
    key = tmp_key;
    keylen = sizeof(tmp_key);
  }

  memset(buf1, 0, sizeof(buf1));
  memset(buf2, 0, sizeof(buf2));
  memcpy(buf1, key, keylen);
  memcpy(buf2, key, keylen);

  for (i = 0; i < sizeof(buf1); i++) {
    buf1[i] ^= 0x36;
    buf2[i] ^= 0x5c;
  }

  mg_sha1_init(&ctx);
  mg_sha1_update(&ctx, buf1, sizeof(buf1));
  mg_sha1_update(&ctx, data, datalen);
  mg_sha1_final(out, &ctx);

  mg_sha1_init(&ctx);
  mg_sha1_update(&ctx, buf2, sizeof(buf2));
  mg_sha1_update(&ctx, out, 20);
  mg_sha1_final(out, &ctx);
}