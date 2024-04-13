int nghttp2_is_fatal(int lib_error_code) {
  return lib_error_code < NGHTTP2_ERR_FATAL;
}