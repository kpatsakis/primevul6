void imap_qualify_path(char *dest, size_t len, struct ImapMbox *mx, char *path)
{
  struct Url url;

  mutt_account_tourl(&mx->account, &url);
  url.path = path;

  url_tostring(&url, dest, len, 0);
}