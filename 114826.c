void ndpi_lru_add_to_cache(struct ndpi_lru_cache *c, u_int32_t key, u_int16_t value) {
  u_int32_t slot = key % c->num_entries;

  c->entries[slot].is_full = 1, c->entries[slot].key = key, c->entries[slot].value = value;
}