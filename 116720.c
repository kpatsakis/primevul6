void imap_cachepath(struct ImapData *idata, const char *mailbox, char *dest, size_t dlen)
{
  char *s = NULL;
  const char *p = mailbox;

  for (s = dest; p && *p && dlen; dlen--)
  {
    if (*p == idata->delim)
    {
      *s = '/';
      /* simple way to avoid collisions with UIDs */
      if (*(p + 1) >= '0' && *(p + 1) <= '9')
      {
        if (--dlen)
          *++s = '_';
      }
    }
    else
      *s = *p;
    p++;
    s++;
  }
  *s = '\0';
}