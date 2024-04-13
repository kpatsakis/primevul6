readconf_retries(void)
{
retry_config **chain = &retries;
retry_config *next;
uschar *p;

while ((p = get_config_line()) != NULL)
  {
  retry_rule **rchain;
  uschar *pp, *error;

  next = store_get(sizeof(retry_config));
  next->next = NULL;
  *chain = next;
  chain = &(next->next);
  next->basic_errno = next->more_errno = 0;
  next->senders = NULL;
  next->rules = NULL;
  rchain = &(next->rules);

  next->pattern = string_dequote(&p);
  while (isspace(*p)) p++;
  pp = p;
  while (mac_isgraph(*p)) p++;
  if (p - pp <= 0) log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "missing error type");

  /* Test error names for things we understand. */

  if ((error = readconf_retry_error(pp, p, &(next->basic_errno),
       &(next->more_errno))) != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "%s", error);

  /* There may be an optional address list of senders to be used as another
  constraint on the rule. This was added later, so the syntax is a bit of a
  fudge. Anything that is not a retry rule starting "F," or "G," is treated as
  an address list. */

  while (isspace(*p)) p++;
  if (Ustrncmp(p, "senders", 7) == 0)
    {
    p += 7;
    while (isspace(*p)) p++;
    if (*p++ != '=') log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "\"=\" expected after \"senders\" in retry rule");
    while (isspace(*p)) p++;
    next->senders = string_dequote(&p);
    }

  /* Now the retry rules. Keep the maximum timeout encountered. */

  while (isspace(*p)) p++;

  while (*p != 0)
    {
    retry_rule *rule = store_get(sizeof(retry_rule));
    *rchain = rule;
    rchain = &(rule->next);
    rule->next = NULL;
    rule->rule = toupper(*p++);
    rule->timeout = retry_arg(&p, 0);
    if (rule->timeout > retry_maximum_timeout)
      retry_maximum_timeout = rule->timeout;

    switch (rule->rule)
      {
      case 'F':   /* Fixed interval */
      rule->p1 = retry_arg(&p, 0);
      break;

      case 'G':   /* Geometrically increasing intervals */
      case 'H':   /* Ditto, but with randomness */
      rule->p1 = retry_arg(&p, 0);
      rule->p2 = retry_arg(&p, 1);
      break;

      default:
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "unknown retry rule letter");
      break;
      }

    if (rule->timeout <= 0 || rule->p1 <= 0 ||
          (rule->rule != 'F' && rule->p2 < 1000))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
        "bad parameters for retry rule");

    while (isspace(*p)) p++;
    if (*p == ';')
      {
      p++;
      while (isspace(*p)) p++;
      }
    else if (*p != 0)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "semicolon expected");
    }
  }
}