int ndpi_is_custom_category(ndpi_protocol_category_t category) {
  switch (category) {
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_1:
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_2:
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_3:
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_4:
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_5:
    return(1);
    break;

  default:
    return(0);
    break;
  }
}