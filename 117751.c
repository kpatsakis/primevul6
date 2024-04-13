static int inflight_settings_new(nghttp2_inflight_settings **settings_ptr,
                                 const nghttp2_settings_entry *iv, size_t niv,
                                 nghttp2_mem *mem) {
  *settings_ptr = nghttp2_mem_malloc(mem, sizeof(nghttp2_inflight_settings));
  if (!*settings_ptr) {
    return NGHTTP2_ERR_NOMEM;
  }

  if (niv > 0) {
    (*settings_ptr)->iv = nghttp2_frame_iv_copy(iv, niv, mem);
    if (!(*settings_ptr)->iv) {
      nghttp2_mem_free(mem, *settings_ptr);
      return NGHTTP2_ERR_NOMEM;
    }
  } else {
    (*settings_ptr)->iv = NULL;
  }

  (*settings_ptr)->niv = niv;
  (*settings_ptr)->next = NULL;

  return 0;
}