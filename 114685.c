struct ndpi_lru_cache *ndpi_lru_cache_init(u_int32_t num_entries) {
  struct ndpi_lru_cache *c = (struct ndpi_lru_cache *) ndpi_malloc(sizeof(struct ndpi_lru_cache));

  if(!c)
    return(NULL);

  c->entries = (struct ndpi_lru_cache_entry *) ndpi_calloc(num_entries, sizeof(struct ndpi_lru_cache_entry));

  if(!c->entries) {
    ndpi_free(c);
    return(NULL);
  } else
    c->num_entries = num_entries;

  return(c);
}