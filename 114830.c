void set_ndpi_debug_function(struct ndpi_detection_module_struct *ndpi_str, ndpi_debug_function_ptr ndpi_debug_printf) {
#ifdef NDPI_ENABLE_DEBUG_MESSAGES
  ndpi_str->ndpi_debug_printf = ndpi_debug_printf;
#endif
}