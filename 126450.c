void mg_md5_final(mg_md5_ctx *ctx, unsigned char digest[16]) {
  unsigned count;
  unsigned char *p;
  uint32_t *a;

  count = (ctx->bits[0] >> 3) & 0x3F;

  p = ctx->in + count;
  *p++ = 0x80;
  count = 64 - 1 - count;
  if (count < 8) {
    memset(p, 0, count);
    mg_byte_reverse(ctx->in, 16);
    mg_md5_transform(ctx->buf, (uint32_t *) ctx->in);
    memset(ctx->in, 0, 56);
  } else {
    memset(p, 0, count - 8);
  }
  mg_byte_reverse(ctx->in, 14);

  a = (uint32_t *) ctx->in;
  a[14] = ctx->bits[0];
  a[15] = ctx->bits[1];

  mg_md5_transform(ctx->buf, (uint32_t *) ctx->in);
  mg_byte_reverse((unsigned char *) ctx->buf, 4);
  memcpy(digest, ctx->buf, 16);
  memset((char *) ctx, 0, sizeof(*ctx));
}