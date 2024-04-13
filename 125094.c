uipbuf_search_header(uint8_t *buffer, uint16_t size, uint8_t protocol)
{
  uint8_t *nbuf;
  uint8_t next_proto;

  nbuf = uipbuf_get_next_header(buffer, size, &next_proto, true);
  while(nbuf != NULL && next_proto != protocol && uip_is_proto_ext_hdr(next_proto)) {
    /* move to the ext hdr */
    nbuf = uipbuf_get_next_header(nbuf, size - (nbuf - buffer), &next_proto, false);
  }

  if(next_proto == protocol) {
    return nbuf;
  } else {
    return NULL;
  }
}