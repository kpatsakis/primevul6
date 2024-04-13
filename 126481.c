static void mg_log_stdout(const void *buf, size_t len, void *userdata) {
  (void) userdata;
#if MG_ENABLE_FILE
  fwrite(buf, 1, len, stdout);
#endif
}