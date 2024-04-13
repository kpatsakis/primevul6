int imap_hcache_del(struct ImapData *idata, unsigned int uid)
{
  char key[16];

  if (!idata->hcache)
    return -1;

  sprintf(key, "/%u", uid);
  return mutt_hcache_delete(idata->hcache, key, imap_hcache_keylen(key));
}