static int session_handle_invalid_stream(nghttp2_session *session,
                                         nghttp2_frame *frame,
                                         int lib_error_code) {
  return session_handle_invalid_stream2(session, frame->hd.stream_id, frame,
                                        lib_error_code);
}