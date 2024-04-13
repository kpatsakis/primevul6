int ndpi_set_detection_preferences(struct ndpi_detection_module_struct *ndpi_str, ndpi_detection_preference pref,
                                   int value) {
  switch (pref) {
  case ndpi_pref_direction_detect_disable:
    ndpi_str->direction_detect_disable = (u_int8_t) value;
    break;

  default:
    return(-1);
  }

  return(0);
}