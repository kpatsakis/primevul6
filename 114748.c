u_int8_t ndpi_detection_get_l4(const u_int8_t *l3, u_int16_t l3_len, const u_int8_t **l4_return,
                               u_int16_t *l4_len_return, u_int8_t *l4_protocol_return, u_int32_t flags) {
  return(ndpi_detection_get_l4_internal(NULL, l3, l3_len, l4_return, l4_len_return, l4_protocol_return, flags));
}