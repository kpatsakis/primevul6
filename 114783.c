void ndpi_lru_free_cache(struct ndpi_lru_cache *c) {
  ndpi_free(c->entries);
  ndpi_free(c);
}