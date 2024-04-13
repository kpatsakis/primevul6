static uint32_t get_error_code_from_lib_error_code(int lib_error_code) {
  switch (lib_error_code) {
  case NGHTTP2_ERR_STREAM_CLOSED:
    return NGHTTP2_STREAM_CLOSED;
  case NGHTTP2_ERR_HEADER_COMP:
    return NGHTTP2_COMPRESSION_ERROR;
  case NGHTTP2_ERR_FRAME_SIZE_ERROR:
    return NGHTTP2_FRAME_SIZE_ERROR;
  case NGHTTP2_ERR_FLOW_CONTROL:
    return NGHTTP2_FLOW_CONTROL_ERROR;
  case NGHTTP2_ERR_REFUSED_STREAM:
    return NGHTTP2_REFUSED_STREAM;
  case NGHTTP2_ERR_PROTO:
  case NGHTTP2_ERR_HTTP_HEADER:
  case NGHTTP2_ERR_HTTP_MESSAGING:
    return NGHTTP2_PROTOCOL_ERROR;
  default:
    return NGHTTP2_INTERNAL_ERROR;
  }
}