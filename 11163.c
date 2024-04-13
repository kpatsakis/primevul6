retry_arg(uschar **paddr, int type)
{
uschar *p = *paddr;
uschar *pp;

if (*p++ != ',') log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "comma expected");

while (isspace(*p)) p++;
pp = p;
while (isalnum(*p) || (type == 1 && *p == '.')) p++;

if (*p != 0 && !isspace(*p) && *p != ',' && *p != ';')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "comma or semicolon expected");

*paddr = p;
switch (type)
  {
  case 0:
  return readconf_readtime(pp, *p, FALSE);
  case 1:
  return readconf_readfixed(pp, *p);
  }
return 0;    /* Keep picky compilers happy */
}