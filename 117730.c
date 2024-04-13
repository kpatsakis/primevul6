static int check_ext_type_set(const uint8_t *ext_types, uint8_t type) {
  return (ext_types[type / 8] & (1 << (type & 0x7))) > 0;
}