static void init_settings(nghttp2_settings_storage *settings) {
  settings->header_table_size = NGHTTP2_HD_DEFAULT_MAX_BUFFER_SIZE;
  settings->enable_push = 1;
  settings->max_concurrent_streams = NGHTTP2_DEFAULT_MAX_CONCURRENT_STREAMS;
  settings->initial_window_size = NGHTTP2_INITIAL_WINDOW_SIZE;
  settings->max_frame_size = NGHTTP2_MAX_FRAME_SIZE_MIN;
  settings->max_header_list_size = UINT32_MAX;
}