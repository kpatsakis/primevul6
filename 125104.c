uipbuf_is_attr_flag(uint16_t flag)
{
  return (uipbuf_attrs[UIPBUF_ATTR_FLAGS] & flag) == flag;
}