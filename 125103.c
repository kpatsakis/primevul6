uipbuf_clear_attr(void)
{
  /* set everything to "defaults" */
  memcpy(uipbuf_attrs, uipbuf_default_attrs, sizeof(uipbuf_attrs));
}