static int ndpi_add_host_url_subprotocol(struct ndpi_detection_module_struct *ndpi_str, char *_value, int protocol_id,
                                         ndpi_protocol_category_t category, ndpi_protocol_breed_t breed) {
  int rv;
  char *value = ndpi_strdup(_value);

  if(!value)
    return(-1);

#ifdef DEBUG
  NDPI_LOG_DBG2(ndpi_str, "[NDPI] Adding [%s][%d]\n", value, protocol_id);
#endif

  rv = ndpi_string_to_automa(ndpi_str, &ndpi_str->host_automa, value, protocol_id, category, breed, 1);

  if(rv != 0)
    ndpi_free(value);

  return(rv);
}