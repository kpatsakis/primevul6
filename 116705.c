struct ImapData *imap_new_idata(void)
{
  struct ImapData *idata = mutt_mem_calloc(1, sizeof(struct ImapData));

  idata->cmdbuf = mutt_buffer_new();
  idata->cmdslots = ImapPipelineDepth + 2;
  idata->cmds = mutt_mem_calloc(idata->cmdslots, sizeof(*idata->cmds));

  STAILQ_INIT(&idata->flags);
  STAILQ_INIT(&idata->mboxcache);

  return idata;
}