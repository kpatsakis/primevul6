readconf_one_rewrite(uschar *p, int *existflags, BOOL isglobal)
{
rewrite_rule *next = store_get(sizeof(rewrite_rule));

next->next = NULL;
next->key = string_dequote(&p);

while (isspace(*p)) p++;
if (*p == 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "missing rewrite replacement string");

next->flags = 0;
next->replacement = string_dequote(&p);

while (*p != 0) switch (*p++)
  {
  case ' ': case '\t': break;

  case 'q': next->flags |= rewrite_quit; break;
  case 'w': next->flags |= rewrite_whole; break;

  case 'h': next->flags |= rewrite_all_headers; break;
  case 's': next->flags |= rewrite_sender; break;
  case 'f': next->flags |= rewrite_from; break;
  case 't': next->flags |= rewrite_to;   break;
  case 'c': next->flags |= rewrite_cc;   break;
  case 'b': next->flags |= rewrite_bcc;  break;
  case 'r': next->flags |= rewrite_replyto; break;

  case 'E': next->flags |= rewrite_all_envelope; break;
  case 'F': next->flags |= rewrite_envfrom; break;
  case 'T': next->flags |= rewrite_envto; break;

  case 'Q': next->flags |= rewrite_qualify; break;
  case 'R': next->flags |= rewrite_repeat; break;

  case 'S':
  next->flags |= rewrite_smtp;
  if (next->key[0] != '^' && Ustrncmp(next->key, "\\N^", 3) != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "rewrite rule has the S flag but is not a regular expression");
  break;

  default:
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "unknown rewrite flag character '%c' "
    "(could be missing quotes round replacement item)", p[-1]);
  break;
  }

/* If no action flags are set, set all the "normal" rewrites. */

if ((next->flags & (rewrite_all | rewrite_smtp)) == 0)
  next->flags |= isglobal? rewrite_all : rewrite_all_headers;

/* Remember which exist, for optimization, and return the rule */

*existflags |= next->flags;
return next;
}