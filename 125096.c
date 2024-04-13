uipbuf_set_attr(uint8_t type, uint16_t value)
{
  if(type < UIPBUF_ATTR_MAX) {
    uipbuf_attrs[type] = value;
    return 1;
  }
  return 0;
}