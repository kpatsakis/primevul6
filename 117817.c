static int session_update_consumed_size(nghttp2_session *session,
                                        int32_t *consumed_size_ptr,
                                        int32_t *recv_window_size_ptr,
                                        uint8_t window_update_queued,
                                        int32_t stream_id, size_t delta_size,
                                        int32_t local_window_size) {
  int32_t recv_size;
  int rv;

  if ((size_t)*consumed_size_ptr > NGHTTP2_MAX_WINDOW_SIZE - delta_size) {
    return nghttp2_session_terminate_session(session,
                                             NGHTTP2_FLOW_CONTROL_ERROR);
  }

  *consumed_size_ptr += (int32_t)delta_size;

  if (window_update_queued == 0) {
    /* recv_window_size may be smaller than consumed_size, because it
       may be decreased by negative value with
       nghttp2_submit_window_update(). */
    recv_size = nghttp2_min(*consumed_size_ptr, *recv_window_size_ptr);

    if (nghttp2_should_send_window_update(local_window_size, recv_size)) {
      rv = nghttp2_session_add_window_update(session, NGHTTP2_FLAG_NONE,
                                             stream_id, recv_size);

      if (rv != 0) {
        return rv;
      }

      *recv_window_size_ptr -= recv_size;
      *consumed_size_ptr -= recv_size;
    }
  }

  return 0;
}