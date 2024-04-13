dirserv_get_nickname_by_digest(const char *digest)
{
  router_status_t *status;
  if (!fingerprint_list)
    return NULL;
  tor_assert(digest);

  status = digestmap_get(fingerprint_list->status_by_digest, digest);
  return status ? status->nickname : NULL;
}