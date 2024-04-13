static int is_non_fatal(int lib_error_code) {
  return lib_error_code < 0 && lib_error_code > NGHTTP2_ERR_FATAL;
}