static void ndpi_validate_protocol_initialization(struct ndpi_detection_module_struct *ndpi_str) {
  int i;

  for (i = 0; i < (int) ndpi_str->ndpi_num_supported_protocols; i++) {
    if(ndpi_str->proto_defaults[i].protoName == NULL) {
      NDPI_LOG_ERR(ndpi_str,
		   "[NDPI] INTERNAL ERROR missing protoName initialization for [protoId=%d]: recovering\n", i);
    } else {
      if((i != NDPI_PROTOCOL_UNKNOWN) &&
	 (ndpi_str->proto_defaults[i].protoCategory == NDPI_PROTOCOL_CATEGORY_UNSPECIFIED)) {
	NDPI_LOG_ERR(ndpi_str,
		     "[NDPI] INTERNAL ERROR missing category [protoId=%d/%s] initialization: recovering\n", i,
		     ndpi_str->proto_defaults[i].protoName ? ndpi_str->proto_defaults[i].protoName : "???");
      }
    }
  }
}