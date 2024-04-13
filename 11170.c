readconf_retry_error(uschar *pp, uschar *p, int *basic_errno, int *more_errno)
{
int len;
uschar *q = pp;
while (q < p && *q != '_') q++;
len = q - pp;

if (len == 5 && strncmpic(pp, US"quota", len) == 0)
  {
  *basic_errno = ERRNO_EXIMQUOTA;
  if (q != p && (*more_errno = readconf_readtime(q+1, *p, FALSE)) < 0)
      return US"bad time value";
  }

else if (len == 7 && strncmpic(pp, US"refused", len) == 0)
  {
  *basic_errno = ECONNREFUSED;
  if (q != p)
    {
    if (strncmpic(q+1, US"MX", p-q-1) == 0) *more_errno = 'M';
    else if (strncmpic(q+1, US"A", p-q-1) == 0) *more_errno = 'A';
    else return US"A or MX expected after \"refused\"";
    }
  }

else if (len == 7 && strncmpic(pp, US"timeout", len) == 0)
  {
  *basic_errno = ETIMEDOUT;
  if (q != p)
    {
    int i;
    int xlen = p - q - 1;
    uschar *x = q + 1;

    static uschar *extras[] =
      { US"A", US"MX", US"connect", US"connect_A",  US"connect_MX" };
    static int values[] =
      { 'A',   'M',    RTEF_CTOUT,  RTEF_CTOUT|'A', RTEF_CTOUT|'M' };

    for (i = 0; i < sizeof(extras)/sizeof(uschar *); i++)
      {
      if (strncmpic(x, extras[i], xlen) == 0)
        {
        *more_errno = values[i];
        break;
        }
      }

    if (i >= sizeof(extras)/sizeof(uschar *))
      {
      if (strncmpic(x, US"DNS", xlen) == 0)
        {
        log_write(0, LOG_MAIN|LOG_PANIC, "\"timeout_dns\" is no longer "
          "available in retry rules (it has never worked) - treated as "
          "\"timeout\"");
        }
      else return US"\"A\", \"MX\", or \"connect\" expected after \"timeout\"";
      }
    }
  }

else if (strncmpic(pp, US"mail_4", 6) == 0 ||
         strncmpic(pp, US"rcpt_4", 6) == 0 ||
         strncmpic(pp, US"data_4", 6) == 0)
  {
  BOOL bad = FALSE;
  int x = 255;                           /* means "any 4xx code" */
  if (p != pp + 8) bad = TRUE; else
    {
    int a = pp[6], b = pp[7];
    if (isdigit(a))
      {
      x = (a - '0') * 10;
      if (isdigit(b)) x += b - '0';
      else if (b == 'x') x += 100;
      else bad = TRUE;
      }
    else if (a != 'x' || b != 'x') bad = TRUE;
    }

  if (bad)
    return string_sprintf("%.4s_4 must be followed by xx, dx, or dd, where "
      "x is literal and d is any digit", pp);

  *basic_errno = (*pp == 'm')? ERRNO_MAIL4XX :
                 (*pp == 'r')? ERRNO_RCPT4XX : ERRNO_DATA4XX;
  *more_errno = x << 8;
  }

else if (len == 4 && strncmpic(pp, US"auth", len) == 0 &&
         strncmpic(q+1, US"failed", p-q-1) == 0)
  *basic_errno = ERRNO_AUTHFAIL;

else if (strncmpic(pp, US"lost_connection", p - pp) == 0)
  *basic_errno = ERRNO_SMTPCLOSED;

else if (strncmpic(pp, US"tls_required", p - pp) == 0)
  *basic_errno = ERRNO_TLSREQUIRED;

else if (len != 1 || Ustrncmp(pp, "*", 1) != 0)
  return string_sprintf("unknown or malformed retry error \"%.*s\"", p-pp, pp);

return NULL;
}