void *ndpi_malloc(size_t size) {
  return(_ndpi_malloc ? _ndpi_malloc(size) : malloc(size));
}