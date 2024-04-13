void set_ndpi_malloc(void *(*__ndpi_malloc)(size_t size)) {
  _ndpi_malloc = __ndpi_malloc;
}