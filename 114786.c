int ndpi_get_category_id(struct ndpi_detection_module_struct *ndpi_str, char *cat) {
  int i;

  for (i = 0; i < NDPI_PROTOCOL_NUM_CATEGORIES; i++) {
    const char *name = ndpi_category_get_name(ndpi_str, i);

    if(strcasecmp(cat, name) == 0)
      return(i);
  }

  return(-1);
}