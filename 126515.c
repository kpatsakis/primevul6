static void walkchunks(struct mg_connection *c, struct mg_http_message *hm,
                       size_t reqlen) {
  size_t off = 0, bl, ll;
  while (off + reqlen < c->recv.len) {
    char *buf = (char *) &c->recv.buf[reqlen];
    size_t memo = c->recv.len;
    size_t cl = get_chunk_length(&buf[off], memo - reqlen - off, &ll);
    // LOG(LL_INFO, ("len %zu off %zu cl %zu ll %zu", len, off, cl, ll));
    if (cl == 0) break;
    hm->chunk = mg_str_n(&buf[off + ll], cl < ll + 2 ? 0 : cl - ll - 2);
    mg_call(c, MG_EV_HTTP_CHUNK, hm);
    // Increase offset only if user has not deleted this chunk
    if (memo == c->recv.len) off += cl;
    if (cl <= 5) {
      // Zero chunk - last one. Prepare body - cut off chunk lengths
      off = bl = 0;
      while (off + reqlen < c->recv.len) {
        char *buf2 = (char *) &c->recv.buf[reqlen];
        size_t memo2 = c->recv.len;
        size_t cl2 = get_chunk_length(&buf2[off], memo2 - reqlen - off, &ll);
        size_t n = cl2 < ll + 2 ? 0 : cl2 - ll - 2;
        memmove(buf2 + bl, buf2 + off + ll, n);
        bl += n;
        off += cl2;
        if (cl2 <= 5) break;
      }
      // LOG(LL_INFO, ("BL->%d del %d off %d", (int) bl, (int) del, (int) off));
      c->recv.len -= off - bl;
      // Set message length to indicate we've received
      // everything, to fire MG_EV_HTTP_MSG
      hm->message.len = bl + reqlen;
      hm->body.len = bl;
      break;
    }
  }
}