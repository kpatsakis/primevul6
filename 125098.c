uipbuf_add_ext_hdr(int16_t len)
{
  if(len + uip_len <= UIP_LINK_MTU && len + uip_len >= 0 && len + uip_ext_len >= 0) {
    uip_ext_len += len;
    uip_len += len;
    return true;
  } else {
    return false;
  }
}