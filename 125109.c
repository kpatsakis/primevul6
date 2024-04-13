uipbuf_clear(void)
{
  uip_len = 0;
  uip_ext_len = 0;
  uip_last_proto = 0;
  uipbuf_clear_attr();
}