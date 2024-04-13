void imap_unmunge_mbox_name(struct ImapData *idata, char *s)
{
  imap_unquote_string(s);

  char *buf = mutt_str_strdup(s);
  if (buf)
  {
    imap_utf_decode(idata, &buf);
    strncpy(s, buf, strlen(s));
  }

  FREE(&buf);
}