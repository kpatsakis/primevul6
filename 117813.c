static void inflight_settings_del(nghttp2_inflight_settings *settings,
                                  nghttp2_mem *mem) {
  if (!settings) {
    return;
  }

  nghttp2_mem_free(mem, settings->iv);
  nghttp2_mem_free(mem, settings);
}