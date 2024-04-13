void *ndpi_calloc(unsigned long count, size_t size) {
  size_t len = count * size;
  void *p = ndpi_malloc(len);

  if(p)
    memset(p, 0, len);

  return(p);
}