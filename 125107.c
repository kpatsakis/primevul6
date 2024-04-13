uipbuf_clr_attr_flag(uint16_t flag)
{
  uipbuf_attrs[UIPBUF_ATTR_FLAGS] &= ~flag;
}