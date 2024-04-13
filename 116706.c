int imap_parse_path(const char *path, struct ImapMbox *mx)
{
  static unsigned short ImapPort = 0;
  static unsigned short ImapsPort = 0;
  struct servent *service = NULL;
  struct Url url;
  char *c = NULL;

  if (!ImapPort)
  {
    service = getservbyname("imap", "tcp");
    if (service)
      ImapPort = ntohs(service->s_port);
    else
      ImapPort = IMAP_PORT;
    mutt_debug(3, "Using default IMAP port %d\n", ImapPort);
  }
  if (!ImapsPort)
  {
    service = getservbyname("imaps", "tcp");
    if (service)
      ImapsPort = ntohs(service->s_port);
    else
      ImapsPort = IMAP_SSL_PORT;
    mutt_debug(3, "Using default IMAPS port %d\n", ImapsPort);
  }

  /* Defaults */
  memset(&mx->account, 0, sizeof(mx->account));
  mx->account.port = ImapPort;
  mx->account.type = MUTT_ACCT_TYPE_IMAP;

  c = mutt_str_strdup(path);
  url_parse(&url, c);
  if (url.scheme == U_IMAP || url.scheme == U_IMAPS)
  {
    if (mutt_account_fromurl(&mx->account, &url) < 0 || !*mx->account.host)
    {
      url_free(&url);
      FREE(&c);
      return -1;
    }

    mx->mbox = mutt_str_strdup(url.path);

    if (url.scheme == U_IMAPS)
      mx->account.flags |= MUTT_ACCT_SSL;

    url_free(&url);
    FREE(&c);
  }
  /* old PINE-compatibility code */
  else
  {
    url_free(&url);
    FREE(&c);
    char tmp[128];
    if (sscanf(path, "{%127[^}]}", tmp) != 1)
      return -1;

    c = strchr(path, '}');
    if (!c)
      return -1;
    else
    {
      /* walk past closing '}' */
      mx->mbox = mutt_str_strdup(c + 1);
    }

    c = strrchr(tmp, '@');
    if (c)
    {
      *c = '\0';
      mutt_str_strfcpy(mx->account.user, tmp, sizeof(mx->account.user));
      mutt_str_strfcpy(tmp, c + 1, sizeof(tmp));
      mx->account.flags |= MUTT_ACCT_USER;
    }

    const int n = sscanf(tmp, "%127[^:/]%127s", mx->account.host, tmp);
    if (n < 1)
    {
      mutt_debug(1, "NULL host in %s\n", path);
      FREE(&mx->mbox);
      return -1;
    }

    if (n > 1)
    {
      if (sscanf(tmp, ":%hu%127s", &(mx->account.port), tmp) >= 1)
        mx->account.flags |= MUTT_ACCT_PORT;
      if (sscanf(tmp, "/%s", tmp) == 1)
      {
        if (mutt_str_strncmp(tmp, "ssl", 3) == 0)
          mx->account.flags |= MUTT_ACCT_SSL;
        else
        {
          mutt_debug(1, "Unknown connection type in %s\n", path);
          FREE(&mx->mbox);
          return -1;
        }
      }
    }
  }

  if ((mx->account.flags & MUTT_ACCT_SSL) && !(mx->account.flags & MUTT_ACCT_PORT))
    mx->account.port = ImapsPort;

  return 0;
}