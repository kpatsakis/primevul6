void mg_log_set_callback(void (*fn)(const void *, size_t, void *), void *fnd) {
  s_fn = fn;
  s_fn_param = fnd;
}