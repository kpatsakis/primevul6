u_int64_t ndpi_bytestream_to_number64(const u_int8_t *str, u_int16_t max_chars_to_read, u_int16_t *bytes_read) {
  u_int64_t val;
  val = 0;
  // cancel if eof, ' ' or line end chars are reached
  while (max_chars_to_read > 0 && *str >= '0' && *str <= '9') {
    val *= 10;
    val += *str - '0';
    str++;
    max_chars_to_read = max_chars_to_read - 1;
    *bytes_read = *bytes_read + 1;
  }
  return(val);
}