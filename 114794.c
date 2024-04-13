int ndpi_add_content_subprotocol(struct ndpi_detection_module_struct *ndpi_str, char *value, int protocol_id,
                                 ndpi_protocol_category_t category, ndpi_protocol_breed_t breed) {
  return(ndpi_string_to_automa(ndpi_str, &ndpi_str->content_automa, value, protocol_id, category, breed, 0));
}