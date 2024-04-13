void ndpi_free(void *ptr) {
  if(_ndpi_free)
    _ndpi_free(ptr);
  else
    free(ptr);
}