void ndpi_category_set_name(struct ndpi_detection_module_struct *ndpi_str,
			    ndpi_protocol_category_t category,
                            char *name) {
  if(!name)
    return;

  switch (category) {
  case NDPI_PROTOCOL_CATEGORY_CUSTOM_1:
    snprintf(ndpi_str->custom_category_labels[0], CUSTOM_CATEGORY_LABEL_LEN, "%s", name);
    break;

  case NDPI_PROTOCOL_CATEGORY_CUSTOM_2:
    snprintf(ndpi_str->custom_category_labels[1], CUSTOM_CATEGORY_LABEL_LEN, "%s", name);
    break;

  case NDPI_PROTOCOL_CATEGORY_CUSTOM_3:
    snprintf(ndpi_str->custom_category_labels[2], CUSTOM_CATEGORY_LABEL_LEN, "%s", name);
    break;

  case NDPI_PROTOCOL_CATEGORY_CUSTOM_4:
    snprintf(ndpi_str->custom_category_labels[3], CUSTOM_CATEGORY_LABEL_LEN, "%s", name);
    break;

  case NDPI_PROTOCOL_CATEGORY_CUSTOM_5:
    snprintf(ndpi_str->custom_category_labels[4], CUSTOM_CATEGORY_LABEL_LEN, "%s", name);
    break;

  default:
    break;
  }
}