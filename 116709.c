void imap_pretty_mailbox(char *path)
{
  struct ImapMbox home, target;
  struct Url url;
  char *delim = NULL;
  int tlen;
  int hlen = 0;
  bool home_match = false;

  if (imap_parse_path(path, &target) < 0)
    return;

  tlen = mutt_str_strlen(target.mbox);
  /* check whether we can do '=' substitution */
  if (mx_is_imap(Folder) && !imap_parse_path(Folder, &home))
  {
    hlen = mutt_str_strlen(home.mbox);
    if (tlen && mutt_account_match(&home.account, &target.account) &&
        (mutt_str_strncmp(home.mbox, target.mbox, hlen) == 0))
    {
      if (hlen == 0)
        home_match = true;
      else if (ImapDelimChars)
      {
        for (delim = ImapDelimChars; *delim != '\0'; delim++)
          if (target.mbox[hlen] == *delim)
            home_match = true;
      }
    }
    FREE(&home.mbox);
  }

  /* do the '=' substitution */
  if (home_match)
  {
    *path++ = '=';
    /* copy remaining path, skipping delimiter */
    if (hlen == 0)
      hlen = -1;
    memcpy(path, target.mbox + hlen + 1, tlen - hlen - 1);
    path[tlen - hlen - 1] = '\0';
  }
  else
  {
    mutt_account_tourl(&target.account, &url);
    url.path = target.mbox;
    /* FIXME: That hard-coded constant is bogus. But we need the actual
     *   size of the buffer from mutt_pretty_mailbox. And these pretty
     *   operations usually shrink the result. Still... */
    url_tostring(&url, path, 1024, 0);
  }

  FREE(&target.mbox);
}