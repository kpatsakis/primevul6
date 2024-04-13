void mg_http_delete_chunk(struct mg_connection *c, struct mg_http_message *hm) {
  struct mg_str ch = hm->chunk;
  if (mg_is_chunked(hm)) {
    ch.len += 4;  // \r\n before and after the chunk
    ch.ptr -= 2;
    while (ch.ptr > hm->body.ptr && *ch.ptr != '\n') ch.ptr--, ch.len++;
  }
  {
    const char *end = &ch.ptr[ch.len];
    size_t n = (size_t) (end - (char *) c->recv.buf);
    if (c->recv.len > n) {
      memmove((char *) ch.ptr, end, (size_t) (c->recv.len - n));
    }
    // LOG(LL_INFO, ("DELETING CHUNK: %zu %zu %zu\n%.*s", c->recv.len, n,
    // ch.len, (int) ch.len, ch.ptr));
  }
  c->recv.len -= ch.len;
}