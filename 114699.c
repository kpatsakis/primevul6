void ndpi_set_bitmask_protocol_detection(char *label, struct ndpi_detection_module_struct *ndpi_str,
                                         const NDPI_PROTOCOL_BITMASK *detection_bitmask, const u_int32_t idx,
                                         u_int16_t ndpi_protocol_id,
                                         void (*func)(struct ndpi_detection_module_struct *,
                                                      struct ndpi_flow_struct *flow),
                                         const NDPI_SELECTION_BITMASK_PROTOCOL_SIZE ndpi_selection_bitmask,
                                         u_int8_t b_save_bitmask_unknow, u_int8_t b_add_detection_bitmask) {
  /*
    Compare specify protocol bitmask with main detection bitmask
  */
  if(NDPI_COMPARE_PROTOCOL_TO_BITMASK(*detection_bitmask, ndpi_protocol_id) != 0) {
#ifdef DEBUG
    NDPI_LOG_DBG2(ndpi_str,
		  "[NDPI] ndpi_set_bitmask_protocol_detection: %s : [callback_buffer] idx= %u, [proto_defaults] "
		  "protocol_id=%u\n",
		  label, idx, ndpi_protocol_id);
#endif

    if(ndpi_str->proto_defaults[ndpi_protocol_id].protoIdx != 0) {
      NDPI_LOG_DBG2(ndpi_str, "[NDPI] Internal error: protocol %s/%u has been already registered\n", label,
		    ndpi_protocol_id);
#ifdef DEBUG
    } else {
      NDPI_LOG_DBG2(ndpi_str, "[NDPI] Adding %s with protocol id %d\n", label, ndpi_protocol_id);
#endif
    }

    /*
      Set function and index protocol within proto_default structure for port protocol detection
      and callback_buffer function for DPI protocol detection
    */
    ndpi_str->proto_defaults[ndpi_protocol_id].protoIdx = idx;
    ndpi_str->proto_defaults[ndpi_protocol_id].func = ndpi_str->callback_buffer[idx].func = func;

    /*
      Set ndpi_selection_bitmask for protocol
    */
    ndpi_str->callback_buffer[idx].ndpi_selection_bitmask = ndpi_selection_bitmask;

    /*
      Reset protocol detection bitmask via NDPI_PROTOCOL_UNKNOWN and than add specify protocol bitmast to callback
      buffer.
    */
    if(b_save_bitmask_unknow)
      NDPI_SAVE_AS_BITMASK(ndpi_str->callback_buffer[idx].detection_bitmask, NDPI_PROTOCOL_UNKNOWN);
    if(b_add_detection_bitmask)
      NDPI_ADD_PROTOCOL_TO_BITMASK(ndpi_str->callback_buffer[idx].detection_bitmask, ndpi_protocol_id);

    NDPI_SAVE_AS_BITMASK(ndpi_str->callback_buffer[idx].excluded_protocol_bitmask, ndpi_protocol_id);
  }
}