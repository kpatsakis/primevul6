u_int16_t ntohs_ndpi_bytestream_to_number(const u_int8_t *str,
					  u_int16_t max_chars_to_read, u_int16_t *bytes_read) {
  u_int16_t val = ndpi_bytestream_to_number(str, max_chars_to_read, bytes_read);
  return(ntohs(val));
}