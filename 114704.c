void set_ndpi_free(void (*__ndpi_free)(void *ptr)) {
  _ndpi_free = __ndpi_free;
}