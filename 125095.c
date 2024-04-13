uipbuf_init(void)
{
  memset(uipbuf_default_attrs, 0, sizeof(uipbuf_default_attrs));
  /* And initialize anything that should be initialized */
  uipbuf_set_default_attr(UIPBUF_ATTR_MAX_MAC_TRANSMISSIONS,
                          UIP_MAX_MAC_TRANSMISSIONS_UNDEFINED);
  /* set the not-set default value - this will cause the MAC layer to
     configure its default */
  uipbuf_set_default_attr(UIPBUF_ATTR_LLSEC_LEVEL,
                          UIPBUF_ATTR_LLSEC_LEVEL_MAC_DEFAULT);
}