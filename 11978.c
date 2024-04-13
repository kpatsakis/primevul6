router_is_active(routerinfo_t *ri, time_t now)
{
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  if (ri->cache_info.published_on < cutoff)
    return 0;
  if (!ri->is_running || !ri->is_valid || ri->is_hibernating)
    return 0;
  return 1;
}