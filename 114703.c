void ndpi_int_change_category(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                              ndpi_protocol_category_t protocol_category) {
  flow->category = protocol_category;
}