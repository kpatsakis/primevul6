void *ndpi_realloc(void *ptr, size_t old_size, size_t new_size) {
  void *ret = ndpi_malloc(new_size);

  if(!ret)
    return(ret);
  else {
    memcpy(ret, ptr, old_size);
    ndpi_free(ptr);
    return(ret);
  }
}