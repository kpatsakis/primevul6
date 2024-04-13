void mg_sha1_update(mg_sha1_ctx *context, const unsigned char *data,
                    size_t len) {
  size_t i, j;

  j = context->count[0];
  if ((context->count[0] += (uint32_t) len << 3) < j) context->count[1]++;
  context->count[1] += (uint32_t)(len >> 29);
  j = (j >> 3) & 63;
  if ((j + len) > 63) {
    memcpy(&context->buffer[j], data, (i = 64 - j));
    mg_sha1_transform(context->state, context->buffer);
    for (; i + 63 < len; i += 64) {
      mg_sha1_transform(context->state, &data[i]);
    }
    j = 0;
  } else
    i = 0;
  memcpy(&context->buffer[j], &data[i], len - i);
}