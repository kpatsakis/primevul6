void imap_allow_reopen(struct Context *ctx)
{
  struct ImapData *idata = NULL;
  if (!ctx || !ctx->data || ctx->magic != MUTT_IMAP)
    return;

  idata = ctx->data;
  if (idata->ctx == ctx)
    idata->reopen |= IMAP_REOPEN_ALLOW;
}