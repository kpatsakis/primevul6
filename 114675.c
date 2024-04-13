static u_int8_t ndpi_is_more_generic_protocol(u_int16_t previous_proto, u_int16_t new_proto) {
  /* Sometimes certificates are more generic than previously identified protocols */

  if((previous_proto == NDPI_PROTOCOL_UNKNOWN) || (previous_proto == new_proto))
    return(0);

  switch (previous_proto) {
  case NDPI_PROTOCOL_WHATSAPP_CALL:
  case NDPI_PROTOCOL_WHATSAPP_FILES:
    if(new_proto == NDPI_PROTOCOL_WHATSAPP)
      return(1);
  }

  return(0);
}