int ndpi_handle_ipv6_extension_headers(struct ndpi_detection_module_struct *ndpi_str, const u_int8_t **l4ptr,
                                       u_int16_t *l4len, u_int8_t *nxt_hdr) {
  while ((*nxt_hdr == 0 || *nxt_hdr == 43 || *nxt_hdr == 44 || *nxt_hdr == 60 || *nxt_hdr == 135 || *nxt_hdr == 59)) {
    u_int16_t ehdr_len;

    // no next header
    if(*nxt_hdr == 59) {
      return(1);
    }
    // fragment extension header has fixed size of 8 bytes and the first byte is the next header type
    if(*nxt_hdr == 44) {
      if(*l4len < 8) {
	return(1);
      }
      *nxt_hdr = (*l4ptr)[0];
      *l4len -= 8;
      (*l4ptr) += 8;
      continue;
    }
    // the other extension headers have one byte for the next header type
    // and one byte for the extension header length in 8 byte steps minus the first 8 bytes
    if(*l4len < 2) {
      return(1);
    }
    ehdr_len = (*l4ptr)[1];
    ehdr_len *= 8;
    ehdr_len += 8;

    if(*l4len < ehdr_len) {
      return(1);
    }
    *nxt_hdr = (*l4ptr)[0];
    *l4len -= ehdr_len;
    (*l4ptr) += ehdr_len;
  }
  return(0);
}