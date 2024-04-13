nghttp2_session_get_hd_inflate_dynamic_table_size(nghttp2_session *session) {
  return nghttp2_hd_inflate_get_dynamic_table_size(&session->hd_inflater);
}