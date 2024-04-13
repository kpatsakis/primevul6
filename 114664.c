void set_ndpi_flow_malloc(void *(*__ndpi_flow_malloc)(size_t size)) {
  _ndpi_flow_malloc = __ndpi_flow_malloc;
}