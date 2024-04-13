dirserv_get_consensus(const char *flavor_name)
{
  if (!cached_consensuses)
    return NULL;
  return strmap_get(cached_consensuses, flavor_name);
}