static Array memcache_build_stats(const memcached_st *ptr,
                                memcached_stat_st *memc_stat,
                                memcached_return_t *ret) {
  char **curr_key;
  char **stat_keys = memcached_stat_get_keys(const_cast<memcached_st*>(ptr),
                                             memc_stat, ret);

  if (*ret != MEMCACHED_SUCCESS) {
    if (stat_keys) {
      free(stat_keys);
    }
    return Array();
  }

  Array return_val = Array::Create();

  for (curr_key = stat_keys; *curr_key; curr_key++) {
    char *mc_val;
    mc_val = memcached_stat_get_value(ptr, memc_stat, *curr_key, ret);
    if (*ret != MEMCACHED_SUCCESS) {
      break;
    }
    return_val.set(String(*curr_key, CopyString),
                   String(mc_val, CopyString));
    free(mc_val);
  }

  free(stat_keys);
  return return_val;
}