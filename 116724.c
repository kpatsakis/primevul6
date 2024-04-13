int imap_account_match(const struct Account *a1, const struct Account *a2)
{
  struct ImapData *a1_idata = imap_conn_find(a1, MUTT_IMAP_CONN_NONEW);
  struct ImapData *a2_idata = imap_conn_find(a2, MUTT_IMAP_CONN_NONEW);
  const struct Account *a1_canon = a1_idata == NULL ? a1 : &a1_idata->conn->account;
  const struct Account *a2_canon = a2_idata == NULL ? a2 : &a2_idata->conn->account;

  return mutt_account_match(a1_canon, a2_canon);
}