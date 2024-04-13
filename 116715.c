int imap_expand_path(char *path, size_t len)
{
  struct ImapMbox mx;
  struct ImapData *idata = NULL;
  struct Url url;
  char fixedpath[LONG_STRING];
  int rc;

  if (imap_parse_path(path, &mx) < 0)
    return -1;

  idata = imap_conn_find(&mx.account, MUTT_IMAP_CONN_NONEW);
  mutt_account_tourl(&mx.account, &url);
  imap_fix_path(idata, mx.mbox, fixedpath, sizeof(fixedpath));
  url.path = fixedpath;

  rc = url_tostring(&url, path, len, U_DECODE_PASSWD);
  FREE(&mx.mbox);

  return rc;
}