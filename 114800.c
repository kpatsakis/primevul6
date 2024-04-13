const char *ndpi_category_get_name(struct ndpi_detection_module_struct *ndpi_str,
				   ndpi_protocol_category_t category) {
  if((!ndpi_str) || (category >= NDPI_PROTOCOL_NUM_CATEGORIES)) {
    static char b[24];

    if(!ndpi_str)
      snprintf(b, sizeof(b), "NULL nDPI");
    else
      snprintf(b, sizeof(b), "Invalid category %d", (int) category);
    return(b);
  }

  if((category >= NDPI_PROTOCOL_CATEGORY_CUSTOM_1) && (category <= NDPI_PROTOCOL_CATEGORY_CUSTOM_5)) {
    switch (category) {
    case NDPI_PROTOCOL_CATEGORY_CUSTOM_1:
      return(ndpi_str->custom_category_labels[0]);
    case NDPI_PROTOCOL_CATEGORY_CUSTOM_2:
      return(ndpi_str->custom_category_labels[1]);
    case NDPI_PROTOCOL_CATEGORY_CUSTOM_3:
      return(ndpi_str->custom_category_labels[2]);
    case NDPI_PROTOCOL_CATEGORY_CUSTOM_4:
      return(ndpi_str->custom_category_labels[3]);
    case NDPI_PROTOCOL_CATEGORY_CUSTOM_5:
      return(ndpi_str->custom_category_labels[4]);
    case NDPI_PROTOCOL_NUM_CATEGORIES:
      return("Code should not use this internal constant");
    default:
      return("Unspecified");
    }
  } else
    return(categories[category]);
}