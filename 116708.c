int imap_hcache_put(struct ImapData *idata, struct Header *h)
{
  char key[16];

  if (!idata->hcache)
    return -1;

  sprintf(key, "/%u", HEADER_DATA(h)->uid);
  return mutt_hcache_store(idata->hcache, key, imap_hcache_keylen(key), h, idata->uid_validity);
}