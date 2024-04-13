char *ndpi_get_proto_breed_name(struct ndpi_detection_module_struct *ndpi_str,
				ndpi_protocol_breed_t breed_id) {
  switch (breed_id) {
  case NDPI_PROTOCOL_SAFE:
    return("Safe");
    break;
  case NDPI_PROTOCOL_ACCEPTABLE:
    return("Acceptable");
    break;
  case NDPI_PROTOCOL_FUN:
    return("Fun");
    break;
  case NDPI_PROTOCOL_UNSAFE:
    return("Unsafe");
    break;
  case NDPI_PROTOCOL_POTENTIALLY_DANGEROUS:
    return("Potentially Dangerous");
    break;
  case NDPI_PROTOCOL_DANGEROUS:
    return("Dangerous");
    break;
  case NDPI_PROTOCOL_UNRATED:
  default:
    return("Unrated");
    break;
  }
}