uipbuf_set_attr_flag(uint16_t flag)
{
  /* Assume only 16-bits for flags now */
  uipbuf_attrs[UIPBUF_ATTR_FLAGS] |= flag;
}