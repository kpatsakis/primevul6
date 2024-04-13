void imap_hcache_close(struct ImapData *idata)
{
  if (!idata->hcache)
    return;

  mutt_hcache_close(idata->hcache);
  idata->hcache = NULL;
}