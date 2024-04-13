void ndpi_flow_free(void *ptr) {
  if(_ndpi_flow_free)
    _ndpi_flow_free(ptr);
  else
    ndpi_free_flow((struct ndpi_flow_struct *) ptr);
}