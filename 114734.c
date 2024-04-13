int ndpi_match_prefix(const u_int8_t *payload,
		      size_t payload_len, const char *str, size_t str_len) {
  int rc = str_len <= payload_len ? memcmp(payload, str, str_len) == 0 : 0;

  return(rc);
}