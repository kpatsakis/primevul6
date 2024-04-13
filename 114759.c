void *ndpi_flow_malloc(size_t size) {
  return(_ndpi_flow_malloc ? _ndpi_flow_malloc(size) : ndpi_malloc(size));
}