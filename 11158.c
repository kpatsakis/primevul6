readconf_rest(BOOL skip_acl)
{
int had = 0;

while(next_section[0] != 0)
  {
  int bit;
  int first = 0;
  int last = sizeof(section_list) / sizeof(uschar *);
  int mid = last/2;
  int n = Ustrlen(next_section);

  if (tolower(next_section[n-1]) != 's') Ustrcpy(next_section+n, "s");

  for (;;)
    {
    int c = strcmpic(next_section, section_list[mid]);
    if (c == 0) break;
    if (c > 0) first = mid + 1; else last = mid;
    if (first >= last)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
        "\"%.*s\" is not a known configuration section name", n, next_section);
    mid = (last + first)/2;
    }

  bit = 1 << mid;
  if (((had ^= bit) & bit) == 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "\"%.*s\" section is repeated in the configuration file", n,
        next_section);

  switch(mid)
    {
    case 0: readconf_acl(skip_acl); break;
    case 1: auths_init(); break;
    case 2: local_scan_init(); break;
    case 3: readconf_retries(); break;
    case 4: readconf_rewrites(); break;
    case 5: route_init(); break;
    case 6: transport_init(); break;
    }
  }

(void)fclose(config_file);
}