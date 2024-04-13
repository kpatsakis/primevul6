readconf_handle_option(uschar *buffer, optionlist *oltop, int last,
  void *data_block, uschar *unknown_txt)
{
int ptr = 0;
int offset = 0;
int n, count, type, value;
int issecure = 0;
uid_t uid;
gid_t gid;
BOOL boolvalue = TRUE;
BOOL freesptr = TRUE;
BOOL extra_condition = FALSE;
optionlist *ol, *ol2;
struct passwd *pw;
void *reset_point;
int intbase = 0;
uschar *inttype = US"";
uschar *sptr;
uschar *s = buffer;
uschar *saved_condition, *strtemp;
uschar **str_target;
uschar name[64];
uschar name2[64];

/* There may be leading spaces; thereafter, we expect an option name starting
with a letter. */

while (isspace(*s)) s++;
if (!isalpha(*s))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "option setting expected: %s", s);

/* Read the name of the option, and skip any subsequent white space. If
it turns out that what we read was "hide", set the flag indicating that
this is a secure option, and loop to read the next word. */

for (n = 0; n < 2; n++)
  {
  while (isalnum(*s) || *s == '_')
    {
    if (ptr < sizeof(name)-1) name[ptr++] = *s;
    s++;
    }
  name[ptr] = 0;
  while (isspace(*s)) s++;
  if (Ustrcmp(name, "hide") != 0) break;
  issecure = opt_secure;
  ptr = 0;
  }

/* Deal with "no_" or "not_" here for booleans */

if (Ustrncmp(name, "no_", 3) == 0)
  {
  boolvalue = FALSE;
  offset = 3;
  }

if (Ustrncmp(name, "not_", 4) == 0)
  {
  boolvalue = FALSE;
  offset = 4;
  }

/* Search the list for the given name. A non-existent name, or an option that
is set twice, is a disaster. */

ol = find_option(name + offset, oltop, last);

if (ol == NULL)
  {
  if (unknown_txt == NULL) return FALSE;
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, CS unknown_txt, name);
  }

if ((ol->type & opt_set) != 0)
  {
  uschar *mname = name;
  if (Ustrncmp(mname, "no_", 3) == 0) mname += 3;
  if (Ustrcmp(mname, "condition") == 0)
    extra_condition = TRUE;
  else
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "\"%s\" option set for the second time", mname);
  }

ol->type |= opt_set | issecure;
type = ol->type & opt_mask;

/* Types with data values must be followed by '='; the "no[t]_" prefix
applies only to boolean values. */

if (type < opt_bool || type > opt_bool_last)
  {
  if (offset != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "negation prefix applied to a non-boolean option");
  if (*s == 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "unexpected end of line (data missing) after %s", name);
  if (*s != '=')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "missing \"=\" after %s", name);
  }

/* If a boolean wasn't preceded by "no[t]_" it can be followed by = and
true/false/yes/no, or, in the case of opt_expanded_bool, a general string that
ultimately expands to one of those values. */

else if (*s != 0 && (offset != 0 || *s != '='))
  extra_chars_error(s, US"boolean option ", name, US"");

/* Skip white space after = */

if (*s == '=') while (isspace((*(++s))));

/* If there is a data block and the opt_public flag is not set, change
the data block pointer to the private options block. */

if (data_block != NULL && (ol->type & opt_public) == 0)
  data_block = (void *)(((driver_instance *)data_block)->options_block);

/* Now get the data according to the type. */

switch (type)
  {
  /* If a string value is not enclosed in quotes, it consists of
  the rest of the current line, verbatim. Otherwise, string escapes
  are processed.

  A transport is specified as a string, which is then looked up in the
  list of transports. A search type is specified as one of a number of
  known strings.

  A set or rewrite rules for a driver is specified as a string, which is
  then parsed into a suitable chain of control blocks.

  Uids and gids are specified as strings which are then looked up in the
  passwd file. Lists of uids and gids are similarly specified as colon-
  separated strings. */

  case opt_stringptr:
  case opt_uid:
  case opt_gid:
  case opt_expand_uid:
  case opt_expand_gid:
  case opt_uidlist:
  case opt_gidlist:
  case opt_rewrite:

  reset_point = sptr = read_string(s, name);

  /* Having read a string, we now have several different ways of using it,
  depending on the data type, so do another switch. If keeping the actual
  string is not required (because it is interpreted), freesptr is set TRUE,
  and at the end we reset the pool. */

  switch (type)
    {
    /* If this was a string, set the variable to point to the new string,
    and set the flag so its store isn't reclaimed. If it was a list of rewrite
    rules, we still keep the string (for printing), and parse the rules into a
    control block and flags word. */

    case opt_stringptr:
    if (data_block == NULL)
      str_target = (uschar **)(ol->value);
    else
      str_target = (uschar **)((uschar *)data_block + (long int)(ol->value));
    if (extra_condition)
      {
      /* We already have a condition, we're conducting a crude hack to let
      multiple condition rules be chained together, despite storing them in
      text form. */
      saved_condition = *str_target;
      strtemp = string_sprintf("${if and{{bool_lax{%s}}{bool_lax{%s}}}}",
          saved_condition, sptr);
      *str_target = string_copy_malloc(strtemp);
      /* TODO(pdp): there is a memory leak here when we set 3 or more
      conditions; I still don't understand the store mechanism enough
      to know what's the safe way to free content from an earlier store.
      AFAICT, stores stack, so freeing an early stored item also stores
      all data alloc'd after it.  If we knew conditions were adjacent,
      we could survive that, but we don't.  So I *think* we need to take
      another bit from opt_type to indicate "malloced"; this seems like
      quite a hack, especially for this one case.  It also means that
      we can't ever reclaim the store from the *first* condition.

      Because we only do this once, near process start-up, I'm prepared to
      let this slide for the time being, even though it rankles.  */
      }
    else
      {
      *str_target = sptr;
      freesptr = FALSE;
      }
    break;

    case opt_rewrite:
    if (data_block == NULL)
      *((uschar **)(ol->value)) = sptr;
    else
      *((uschar **)((uschar *)data_block + (long int)(ol->value))) = sptr;
    freesptr = FALSE;
    if (type == opt_rewrite)
      {
      int sep = 0;
      int *flagptr;
      uschar *p = sptr;
      rewrite_rule **chain;
      optionlist *ol3;

      sprintf(CS name2, "*%.50s_rules", name);
      ol2 = find_option(name2, oltop, last);
      sprintf(CS name2, "*%.50s_flags", name);
      ol3 = find_option(name2, oltop, last);

      if (ol2 == NULL || ol3 == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
          "rewrite rules not available for driver");

      if (data_block == NULL)
        {
        chain = (rewrite_rule **)(ol2->value);
        flagptr = (int *)(ol3->value);
        }
      else
        {
        chain = (rewrite_rule **)((uschar *)data_block + (long int)(ol2->value));
        flagptr = (int *)((uschar *)data_block + (long int)(ol3->value));
        }

      while ((p = string_nextinlist(&sptr, &sep, big_buffer, BIG_BUFFER_SIZE))
              != NULL)
        {
        rewrite_rule *next = readconf_one_rewrite(p, flagptr, FALSE);
        *chain = next;
        chain = &(next->next);
        }

      if ((*flagptr & (rewrite_all_envelope | rewrite_smtp)) != 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "rewrite rule specifies a "
          "non-header rewrite - not allowed at transport time -");
      }
    break;

    /* If it was an expanded uid, see if there is any expansion to be
    done by checking for the presence of a $ character. If there is, save it
    in the corresponding *expand_user option field. Otherwise, fall through
    to treat it as a fixed uid. Ensure mutual exclusivity of the two kinds
    of data. */

    case opt_expand_uid:
    sprintf(CS name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      uschar *ss = (Ustrchr(sptr, '$') != NULL)? sptr : NULL;

      if (data_block == NULL)
        *((uschar **)(ol2->value)) = ss;
      else
        *((uschar **)((uschar *)data_block + (long int)(ol2->value))) = ss;

      if (ss != NULL)
        {
        *(get_set_flag(name, oltop, last, data_block)) = FALSE;
        freesptr = FALSE;
        break;
        }
      }

    /* Look up a fixed uid, and also make use of the corresponding gid
    if a passwd entry is returned and the gid has not been set. */

    case opt_uid:
    if (!route_finduser(sptr, &pw, &uid))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "user %s was not found", sptr);
    if (data_block == NULL)
      *((uid_t *)(ol->value)) = uid;
    else
      *((uid_t *)((uschar *)data_block + (long int)(ol->value))) = uid;

    /* Set the flag indicating a fixed value is set */

    *(get_set_flag(name, oltop, last, data_block)) = TRUE;

    /* Handle matching gid if we have a passwd entry: done by finding the
    same name with terminating "user" changed to "group"; if not found,
    ignore. Also ignore if the value is already set. */

    if (pw == NULL) break;
    Ustrcpy(name+Ustrlen(name)-4, "group");
    ol2 = find_option(name, oltop, last);
    if (ol2 != NULL && ((ol2->type & opt_mask) == opt_gid ||
        (ol2->type & opt_mask) == opt_expand_gid))
      {
      BOOL *set_flag = get_set_flag(name, oltop, last, data_block);
      if (! *set_flag)
        {
        if (data_block == NULL)
          *((gid_t *)(ol2->value)) = pw->pw_gid;
        else
          *((gid_t *)((uschar *)data_block + (long int)(ol2->value))) = pw->pw_gid;
        *set_flag = TRUE;
        }
      }
    break;

    /* If it was an expanded gid, see if there is any expansion to be
    done by checking for the presence of a $ character. If there is, save it
    in the corresponding *expand_user option field. Otherwise, fall through
    to treat it as a fixed gid. Ensure mutual exclusivity of the two kinds
    of data. */

    case opt_expand_gid:
    sprintf(CS name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      uschar *ss = (Ustrchr(sptr, '$') != NULL)? sptr : NULL;

      if (data_block == NULL)
        *((uschar **)(ol2->value)) = ss;
      else
        *((uschar **)((uschar *)data_block + (long int)(ol2->value))) = ss;

      if (ss != NULL)
        {
        *(get_set_flag(name, oltop, last, data_block)) = FALSE;
        freesptr = FALSE;
        break;
        }
      }

    /* Handle freestanding gid */

    case opt_gid:
    if (!route_findgroup(sptr, &gid))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "group %s was not found", sptr);
    if (data_block == NULL)
      *((gid_t *)(ol->value)) = gid;
    else
      *((gid_t *)((uschar *)data_block + (long int)(ol->value))) = gid;
    *(get_set_flag(name, oltop, last, data_block)) = TRUE;
    break;

    /* If it was a uid list, look up each individual entry, and build
    a vector of uids, with a count in the first element. Put the vector
    in malloc store so we can free the string. (We are reading into
    permanent store already.) */

    case opt_uidlist:
      {
      int count = 1;
      uid_t *list;
      int ptr = 0;
      uschar *p;
      uschar *op = expand_string (sptr);

      if (op == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "failed to expand %s: %s",
          name, expand_string_message);

      p = op;
      if (*p != 0) count++;
      while (*p != 0) if (*p++ == ':' && *p != 0) count++;
      list = store_malloc(count*sizeof(uid_t));
      list[ptr++] = (uid_t)(count - 1);

      if (data_block == NULL)
        *((uid_t **)(ol->value)) = list;
      else
        *((uid_t **)((uschar *)data_block + (long int)(ol->value))) = list;

      p = op;
      while (count-- > 1)
        {
        int sep = 0;
        (void)string_nextinlist(&p, &sep, big_buffer, BIG_BUFFER_SIZE);
        if (!route_finduser(big_buffer, NULL, &uid))
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "user %s was not found",
            big_buffer);
        list[ptr++] = uid;
        }
      }
    break;

    /* If it was a gid list, look up each individual entry, and build
    a vector of gids, with a count in the first element. Put the vector
    in malloc store so we can free the string. (We are reading into permanent
    store already.) */

    case opt_gidlist:
      {
      int count = 1;
      gid_t *list;
      int ptr = 0;
      uschar *p;
      uschar *op = expand_string (sptr);

      if (op == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "failed to expand %s: %s",
          name, expand_string_message);

      p = op;
      if (*p != 0) count++;
      while (*p != 0) if (*p++ == ':' && *p != 0) count++;
      list = store_malloc(count*sizeof(gid_t));
      list[ptr++] = (gid_t)(count - 1);

      if (data_block == NULL)
        *((gid_t **)(ol->value)) = list;
      else
        *((gid_t **)((uschar *)data_block + (long int)(ol->value))) = list;

      p = op;
      while (count-- > 1)
        {
        int sep = 0;
        (void)string_nextinlist(&p, &sep, big_buffer, BIG_BUFFER_SIZE);
        if (!route_findgroup(big_buffer, &gid))
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "group %s was not found",
            big_buffer);
        list[ptr++] = gid;
        }
      }
    break;
    }

  /* Release store if the value of the string doesn't need to be kept. */

  if (freesptr) store_reset(reset_point);
  break;

  /* Expanded boolean: if no characters follow, or if there are no dollar
  characters, this is a fixed-valued boolean, and we fall through. Otherwise,
  save the string for later expansion in the alternate place. */

  case opt_expand_bool:
  if (*s != 0 && Ustrchr(s, '$') != 0)
    {
    sprintf(CS name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      reset_point = sptr = read_string(s, name);
      if (data_block == NULL)
        *((uschar **)(ol2->value)) = sptr;
      else
        *((uschar **)((uschar *)data_block + (long int)(ol2->value))) = sptr;
      freesptr = FALSE;
      break;
      }
    }
  /* Fall through */

  /* Boolean: if no characters follow, the value is boolvalue. Otherwise
  look for yes/not/true/false. Some booleans are stored in a single bit in
  a single int. There's a special fudge for verify settings; without a suffix
  they set both xx_sender and xx_recipient. The table points to the sender
  value; search subsequently for the recipient. There's another special case:
  opt_bool_set also notes when a boolean has been set. */

  case opt_bool:
  case opt_bit:
  case opt_bool_verify:
  case opt_bool_set:
  if (*s != 0)
    {
    s = readconf_readname(name2, 64, s);
    if (strcmpic(name2, US"true") == 0 || strcmpic(name2, US"yes") == 0)
      boolvalue = TRUE;
    else if (strcmpic(name2, US"false") == 0 || strcmpic(name2, US"no") == 0)
      boolvalue = FALSE;
    else log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "\"%s\" is not a valid value for the \"%s\" option", name2, name);
    if (*s != 0) extra_chars_error(s, string_sprintf("\"%s\" ", name2),
      US"for boolean option ", name);
    }

  /* Handle single-bit type. */

  if (type == opt_bit)
    {
    int bit = 1 << ((ol->type >> 16) & 31);
    int *ptr = (data_block == NULL)?
      (int *)(ol->value) :
      (int *)((uschar *)data_block + (long int)ol->value);
    if (boolvalue) *ptr |= bit; else *ptr &= ~bit;
    break;
    }

  /* Handle full BOOL types */

  if (data_block == NULL)
    *((BOOL *)(ol->value)) = boolvalue;
  else
    *((BOOL *)((uschar *)data_block + (long int)(ol->value))) = boolvalue;

  /* Verify fudge */

  if (type == opt_bool_verify)
    {
    sprintf(CS name2, "%.50s_recipient", name + offset);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      if (data_block == NULL)
        *((BOOL *)(ol2->value)) = boolvalue;
      else
        *((BOOL *)((uschar *)data_block + (long int)(ol2->value))) = boolvalue;
      }
    }

  /* Note that opt_bool_set type is set, if there is somewhere to do so */

  else if (type == opt_bool_set)
    {
    sprintf(CS name2, "*set_%.50s", name + offset);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      if (data_block == NULL)
        *((BOOL *)(ol2->value)) = TRUE;
      else
        *((BOOL *)((uschar *)data_block + (long int)(ol2->value))) = TRUE;
      }
    }
  break;

  /* Octal integer */

  case opt_octint:
  intbase = 8;
  inttype = US"octal ";

  /*  Integer: a simple(ish) case; allow octal and hex formats, and
  suffixes K and M. The different types affect output, not input. */

  case opt_mkint:
  case opt_int:
    {
    uschar *endptr;
    long int lvalue;

    errno = 0;
    lvalue = strtol(CS s, CSS &endptr, intbase);

    if (endptr == s)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "%sinteger expected for %s",
        inttype, name);

    if (errno != ERANGE)
      {
      if (tolower(*endptr) == 'k')
        {
        if (lvalue > INT_MAX/1024 || lvalue < INT_MIN/1024) errno = ERANGE;
          else lvalue *= 1024;
        endptr++;
        }
      else if (tolower(*endptr) == 'm')
        {
        if (lvalue > INT_MAX/(1024*1024) || lvalue < INT_MIN/(1024*1024))
          errno = ERANGE;
        else lvalue *= 1024*1024;
        endptr++;
        }
      }

    if (errno == ERANGE || lvalue > INT_MAX || lvalue < INT_MIN)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
        "absolute value of integer \"%s\" is too large (overflow)", s);

    while (isspace(*endptr)) endptr++;
    if (*endptr != 0)
      extra_chars_error(endptr, inttype, US"integer value for ", name);

    value = (int)lvalue;
    }

  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((uschar *)data_block + (long int)(ol->value))) = value;
  break;

  /*  Integer held in K: again, allow octal and hex formats, and suffixes K and
  M. */

  case opt_Kint:
    {
    uschar *endptr;
    errno = 0;
    value = strtol(CS s, CSS &endptr, intbase);

    if (endptr == s)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "%sinteger expected for %s",
        inttype, name);

    if (errno != ERANGE)
      {
      if (tolower(*endptr) == 'm')
        {
        if (value > INT_MAX/1024 || value < INT_MIN/1024) errno = ERANGE;
          else value *= 1024;
        endptr++;
        }
      else if (tolower(*endptr) == 'k')
        {
        endptr++;
        }
      else
        {
        value = (value + 512)/1024;
        }
      }

    if (errno == ERANGE) log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "absolute value of integer \"%s\" is too large (overflow)", s);

    while (isspace(*endptr)) endptr++;
    if (*endptr != 0)
      extra_chars_error(endptr, inttype, US"integer value for ", name);
    }

  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((uschar *)data_block + (long int)(ol->value))) = value;
  break;

  /*  Fixed-point number: held to 3 decimal places. */

  case opt_fixed:
  if (sscanf(CS s, "%d%n", &value, &count) != 1)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "fixed-point number expected for %s", name);

  if (value < 0) log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "integer \"%s\" is too large (overflow)", s);

  value *= 1000;

  if (value < 0) log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "integer \"%s\" is too large (overflow)", s);

  if (s[count] == '.')
    {
    int d = 100;
    while (isdigit(s[++count]))
      {
      value += (s[count] - '0') * d;
      d /= 10;
      }
    }

  while (isspace(s[count])) count++;

  if (s[count] != 0)
    extra_chars_error(s+count, US"fixed-point value for ", name, US"");

  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((uschar *)data_block + (long int)(ol->value))) = value;
  break;

  /* There's a special routine to read time values. */

  case opt_time:
  value = readconf_readtime(s, 0, FALSE);
  if (value < 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "invalid time value for %s",
      name);
  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((uschar *)data_block + (long int)(ol->value))) = value;
  break;

  /* A time list is a list of colon-separated times, with the first
  element holding the size of the list and the second the number of
  entries used. */

  case opt_timelist:
    {
    int count = 0;
    int *list = (data_block == NULL)?
      (int *)(ol->value) :
      (int *)((uschar *)data_block + (long int)(ol->value));

    if (*s != 0) for (count = 1; count <= list[0] - 2; count++)
      {
      int terminator = 0;
      uschar *snext = Ustrchr(s, ':');
      if (snext != NULL)
        {
        uschar *ss = snext;
        while (ss > s && isspace(ss[-1])) ss--;
        terminator = *ss;
        }
      value = readconf_readtime(s, terminator, FALSE);
      if (value < 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "invalid time value for %s",
          name);
      if (count > 1 && value <= list[count])
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
          "time value out of order for %s", name);
      list[count+1] = value;
      if (snext == NULL) break;
      s = snext + 1;
      while (isspace(*s)) s++;
      }

    if (count > list[0] - 2)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "too many time values for %s",
        name);
    if (count > 0 && list[2] == 0) count = 0;
    list[1] = count;
    }

  break;
  }

return TRUE;
}