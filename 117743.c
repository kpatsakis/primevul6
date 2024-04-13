nghttp2_session_enforce_flow_control_limits(nghttp2_session *session,
                                            nghttp2_stream *stream,
                                            ssize_t requested_window_size) {
  DEBUGF("send: remote windowsize connection=%d, remote maxframsize=%u, "
         "stream(id %d)=%d\n",
         session->remote_window_size, session->remote_settings.max_frame_size,
         stream->stream_id, stream->remote_window_size);

  return nghttp2_min(nghttp2_min(nghttp2_min(requested_window_size,
                                             stream->remote_window_size),
                                 session->remote_window_size),
                     (int32_t)session->remote_settings.max_frame_size);
}