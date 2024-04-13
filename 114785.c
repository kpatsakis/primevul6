void ndpi_exclude_protocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                           u_int16_t protocol_id, const char *_file, const char *_func, int _line) {
  if(protocol_id < NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS) {
#ifdef NDPI_ENABLE_DEBUG_MESSAGES
    if(ndpi_str && ndpi_str->ndpi_log_level >= NDPI_LOG_DEBUG && ndpi_str->ndpi_debug_printf != NULL) {
      (*(ndpi_str->ndpi_debug_printf))(protocol_id, ndpi_str, NDPI_LOG_DEBUG, _file, _func, _line, "exclude %s\n",
				       ndpi_get_proto_name(ndpi_str, protocol_id));
    }
#endif
    NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, protocol_id);
  }
}