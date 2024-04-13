struct Header *imap_hcache_get(struct ImapData *idata, unsigned int uid)
{
  char key[16];
  void *uv = NULL;
  struct Header *h = NULL;

  if (!idata->hcache)
    return NULL;

  sprintf(key, "/%u", uid);
  uv = mutt_hcache_fetch(idata->hcache, key, imap_hcache_keylen(key));
  if (uv)
  {
    if (*(unsigned int *) uv == idata->uid_validity)
      h = mutt_hcache_restore(uv);
    else
      mutt_debug(3, "hcache uidvalidity mismatch: %u\n", *(unsigned int *) uv);
    mutt_hcache_free(idata->hcache, &uv);
  }

  return h;
}