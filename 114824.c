void set_ndpi_flow_free(void (*__ndpi_flow_free)(void *ptr)) {
  _ndpi_flow_free = __ndpi_flow_free;
}