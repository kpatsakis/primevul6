void imap_free_idata(struct ImapData **idata)
{
  if (!idata)
    return;

  FREE(&(*idata)->capstr);
  mutt_list_free(&(*idata)->flags);
  imap_mboxcache_free(*idata);
  mutt_buffer_free(&(*idata)->cmdbuf);
  FREE(&(*idata)->buf);
  mutt_bcache_close(&(*idata)->bcache);
  FREE(&(*idata)->cmds);
  FREE(idata);
}