uipbuf_set_len(uint16_t len)
{
  if(len <= UIP_LINK_MTU) {
    uip_len = len;
    return true;
  } else {
    return false;
  }
}