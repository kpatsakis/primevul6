u_int8_t ndpi_lru_find_cache(struct ndpi_lru_cache *c, u_int32_t key,
			     u_int16_t *value, u_int8_t clean_key_when_found) {
  u_int32_t slot = key % c->num_entries;

  if(c->entries[slot].is_full) {
    *value = c->entries[slot].value;
    if(clean_key_when_found)
      c->entries[slot].is_full = 0;
    return(1);
  } else
    return(0);
}