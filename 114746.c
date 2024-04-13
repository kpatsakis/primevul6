void ndpi_debug_printf(unsigned int proto, struct ndpi_detection_module_struct *ndpi_str, ndpi_log_level_t log_level,
                       const char *file_name, const char *func_name, int line_number, const char *format, ...) {
#ifdef NDPI_ENABLE_DEBUG_MESSAGES
  va_list args;
#define MAX_STR_LEN 250
  char str[MAX_STR_LEN];
  if(ndpi_str != NULL && log_level > NDPI_LOG_ERROR && proto > 0 && proto < NDPI_MAX_SUPPORTED_PROTOCOLS &&
     !NDPI_ISSET(&ndpi_str->debug_bitmask, proto))
    return;
  va_start(args, format);
  vsnprintf(str, sizeof(str) - 1, format, args);
  va_end(args);

  if(ndpi_str != NULL) {
    printf("%s:%s:%-3d - [%s]: %s", file_name, func_name, line_number, ndpi_get_proto_name(ndpi_str, proto), str);
  } else {
    printf("Proto: %u, %s", proto, str);
  }
#endif
}