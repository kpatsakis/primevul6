uipbuf_get_attr(uint8_t type)
{
  if(type < UIPBUF_ATTR_MAX) {
    return uipbuf_attrs[type];
  }
  return 0;
}