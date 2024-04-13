void imap_keepalive(void)
{
  struct Connection *conn = NULL;
  struct ImapData *idata = NULL;
  time_t now = time(NULL);

  TAILQ_FOREACH(conn, mutt_socket_head(), entries)
  {
    if (conn->account.type == MUTT_ACCT_TYPE_IMAP)
    {
      idata = conn->data;
      if (idata->state >= IMAP_AUTHENTICATED && now >= idata->lastread + ImapKeepalive)
      {
        imap_check(idata, 1);
      }
    }
  }
}