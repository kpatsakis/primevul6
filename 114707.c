void ndpi_debug_get_last_log_function_line(struct ndpi_detection_module_struct *ndpi_str, const char **file,
                                           const char **func, u_int32_t *line) {
  *file = "";
  *func = "";

  if(ndpi_str->ndpi_debug_print_file != NULL)
    *file = ndpi_str->ndpi_debug_print_file;

  if(ndpi_str->ndpi_debug_print_function != NULL)
    *func = ndpi_str->ndpi_debug_print_function;

  *line = ndpi_str->ndpi_debug_print_line;
}