uipbuf_get_len_field(struct uip_ip_hdr *hdr)
{
  return ((uint16_t)(hdr->len[0]) << 8) + hdr->len[1];
}