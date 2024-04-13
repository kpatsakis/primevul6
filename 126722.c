void mg_log_set(const char *spec) {
  LOG(LL_DEBUG, ("Setting log level to %s", spec));
  s_spec = spec;
}