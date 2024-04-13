int ndpi_add_string_to_automa(void *_automa, char *str) {
  return(ndpi_add_string_value_to_automa(_automa, str, 1));
}