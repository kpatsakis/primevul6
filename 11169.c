unpick_ratelimit(uschar *s, int *threshold, int *base, double *factor,
  int *limit)
{
uschar bstring[16], lstring[16];

if (sscanf(CS s, "%d, %15[0123456789smhdw.], %lf, %15s", threshold, bstring,
    factor, lstring) == 4)
  {
  *base = readconf_readtime(bstring, 0, TRUE);
  *limit = readconf_readtime(lstring, 0, TRUE);
  if (*base >= 0 && *limit >= 0) return;
  }
log_write(0, LOG_MAIN|LOG_PANIC_DIE, "malformed ratelimit data: %s", s);
}