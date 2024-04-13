uipbuf_set_len_field(struct uip_ip_hdr *hdr, uint16_t len)
{
  hdr->len[0] = (len >> 8);
  hdr->len[1] = (len & 0xff);
}