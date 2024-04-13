get_config_line(void)
{
int startoffset = 0;         /* To first non-blank char in logical line */
int len = 0;                 /* Of logical line so far */
int newlen;
uschar *s, *ss;
macro_item *m;
BOOL macro_found;

/* Loop for handling continuation lines, skipping comments, and dealing with
.include files. */

for (;;)
  {
  if (Ufgets(big_buffer+len, big_buffer_size-len, config_file) == NULL)
    {
    if (config_file_stack != NULL)    /* EOF inside .include */
      {
      (void)fclose(config_file);
      config_file = config_file_stack->file;
      config_filename = config_file_stack->filename;
      config_lineno = config_file_stack->lineno;
      config_file_stack = config_file_stack->next;
      continue;
      }

    /* EOF at top level */

    if (cstate_stack_ptr >= 0)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
        "Unexpected end of configuration file: .endif missing");

    if (len != 0) break;        /* EOF after continuation */
    next_section[0] = 0;        /* EOF at start of logical line */
    return NULL;
    }

  config_lineno++;
  newlen = len + Ustrlen(big_buffer + len);

  /* Handle pathologically long physical lines - yes, it did happen - by
  extending big_buffer at this point. The code also copes with very long
  logical lines. */

  while (newlen == big_buffer_size - 1 && big_buffer[newlen - 1] != '\n')
    {
    uschar *newbuffer;
    big_buffer_size += BIG_BUFFER_SIZE;
    newbuffer = store_malloc(big_buffer_size);

    /* This use of strcpy is OK because we know that the string in the old
    buffer is shorter than the new buffer. */

    Ustrcpy(newbuffer, big_buffer);
    store_free(big_buffer);
    big_buffer = newbuffer;
    if (Ufgets(big_buffer+newlen, big_buffer_size-newlen, config_file) == NULL)
      break;
    newlen += Ustrlen(big_buffer + newlen);
    }

  /* Find the true start of the physical line - leading spaces are always
  ignored. */

  ss = big_buffer + len;
  while (isspace(*ss)) ss++;

  /* Process the physical line for macros. If this is the start of the logical
  line, skip over initial text at the start of the line if it starts with an
  upper case character followed by a sequence of name characters and an equals
  sign, because that is the definition of a new macro, and we don't do
  replacement therein. */

  s = ss;
  if (len == 0 && isupper(*s))
    {
    while (isalnum(*s) || *s == '_') s++;
    while (isspace(*s)) s++;
    if (*s != '=') s = ss;          /* Not a macro definition */
    }

  /* For each defined macro, scan the line (from after XXX= if present),
  replacing all occurrences of the macro. */

  macro_found = FALSE;
  for (m = macros; m != NULL; m = m->next)
    {
    uschar *p, *pp;
    uschar *t = s;

    while ((p = Ustrstr(t, m->name)) != NULL)
      {
      int moveby;
      int namelen = Ustrlen(m->name);
      int replen = Ustrlen(m->replacement);

      /* Expand the buffer if necessary */

      while (newlen - namelen + replen + 1 > big_buffer_size)
        {
        int newsize = big_buffer_size + BIG_BUFFER_SIZE;
        uschar *newbuffer = store_malloc(newsize);
        memcpy(newbuffer, big_buffer, newlen + 1);
        p = newbuffer  + (p - big_buffer);
        s = newbuffer  + (s - big_buffer);
        ss = newbuffer + (ss - big_buffer);
        t = newbuffer  + (t - big_buffer);
        big_buffer_size = newsize;
        store_free(big_buffer);
        big_buffer = newbuffer;
        }

      /* Shuffle the remaining characters up or down in the buffer before
      copying in the replacement text. Don't rescan the replacement for this
      same macro. */

      pp = p + namelen;
      moveby = replen - namelen;
      if (moveby != 0)
        {
        memmove(p + replen, pp, (big_buffer + newlen) - pp + 1);
        newlen += moveby;
        }
      Ustrncpy(p, m->replacement, replen);
      t = p + replen;
      macro_found = TRUE;
      }
    }

  /* An empty macro replacement at the start of a line could mean that ss no
  longer points to the first non-blank character. */

  while (isspace(*ss)) ss++;

  /* Check for comment lines - these are physical lines. */

  if (*ss == '#') continue;

  /* Handle conditionals, which are also applied to physical lines. Conditions
  are of the form ".ifdef ANYTEXT" and are treated as true if any macro
  expansion occured on the rest of the line. A preliminary test for the leading
  '.' saves effort on most lines. */

  if (*ss == '.')
    {
    int i;

    /* Search the list of conditional directives */

    for (i = 0; i < cond_list_size; i++)
      {
      int n;
      cond_item *c = cond_list+i;
      if (Ustrncmp(ss+1, c->name, c->namelen) != 0) continue;

      /* The following character must be white space or end of string */

      n = ss[1 + c->namelen];
      if (n != ' ' && n != 't' && n != '\n' && n != 0) break;

      /* .ifdef and .ifndef push the current state onto the stack, then set
      a new one from the table. Stack overflow is an error */

      if (c->pushpop > 0)
        {
        if (cstate_stack_ptr >= CSTATE_STACK_SIZE - 1)
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
            ".%s nested too deeply", c->name);
        cstate_stack[++cstate_stack_ptr] = cstate;
        cstate = next_cstate[cstate][macro_found? c->action1 : c->action2];
        }

      /* For any of the others, stack underflow is an error. The next state
      comes either from the stack (.endif) or from the table. */

      else
        {
        if (cstate_stack_ptr < 0)
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
            ".%s without matching .ifdef", c->name);
        cstate = (c->pushpop < 0)? cstate_stack[cstate_stack_ptr--] :
          next_cstate[cstate][macro_found? c->action1 : c->action2];
        }

      /* Having dealt with a directive, break the loop */

      break;
      }

    /* If we have handled a conditional directive, continue with the next
    physical line. Otherwise, fall through. */

    if (i < cond_list_size) continue;
    }

  /* If the conditional state is not 0 (actively using these lines), ignore
  this input line. */

  if (cstate != 0) continue;  /* Conditional skip */

  /* Handle .include lines - these are also physical lines. */

  if (Ustrncmp(ss, ".include", 8) == 0 &&
       (isspace(ss[8]) ||
         (Ustrncmp(ss+8, "_if_exists", 10) == 0 && isspace(ss[18]))))
    {
    uschar *t;
    int include_if_exists = isspace(ss[8])? 0 : 10;
    config_file_item *save;
    struct stat statbuf;

    ss += 9 + include_if_exists;
    while (isspace(*ss)) ss++;
    t = ss + Ustrlen(ss);
    while (t > ss && isspace(t[-1])) t--;
    if (*ss == '\"' && t[-1] == '\"')
      {
      ss++;
      t--;
      }
    *t = 0;

    if (*ss != '/')
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, ".include specifies a non-"
        "absolute path \"%s\"", ss);

    if (include_if_exists != 0 && (Ustat(ss, &statbuf) != 0)) continue;

    save = store_get(sizeof(config_file_item));
    save->next = config_file_stack;
    config_file_stack = save;
    save->file = config_file;
    save->filename = config_filename;
    save->lineno = config_lineno;

    config_file = Ufopen(ss, "rb");
    if (config_file == NULL)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "failed to open included "
        "configuration file %s", ss);
    config_filename = string_copy(ss);
    config_lineno = 0;
    continue;
    }

  /* If this is the start of the logical line, remember where the non-blank
  data starts. Otherwise shuffle down continuation lines to remove leading
  white space. */

  if (len == 0)
    startoffset = ss - big_buffer;
  else
    {
    s = big_buffer + len;
    if (ss > s)
      {
      memmove(s, ss, (newlen - len) -  (ss - s) + 1);
      newlen -= ss - s;
      }
    }

  /* Accept the new addition to the line. Remove trailing white space. */

  len = newlen;
  while (len > 0 && isspace(big_buffer[len-1])) len--;
  big_buffer[len] = 0;

  /* We are done if the line does not end in backslash and contains some data.
  Empty logical lines are ignored. For continuations, remove the backslash and
  go round the loop to read the continuation line. */

  if (len > 0)
    {
    if (big_buffer[len-1] != '\\') break;   /* End of logical line */
    big_buffer[--len] = 0;                  /* Remove backslash */
    }
  }     /* Loop for reading multiple physical lines */

/* We now have a logical line. Test for the end of a configuration section (or,
more accurately, for the start of the next section). Place the name of the next
section in next_section, and return NULL. If the name given is longer than
next_section, truncate it. It will be unrecognized later, because all the known
section names do fit. Leave space for pluralizing. */

s = big_buffer + startoffset;            /* First non-space character */
if (strncmpic(s, US"begin ", 6) == 0)
  {
  s += 6;
  while (isspace(*s)) s++;
  if (big_buffer + len - s > sizeof(next_section) - 2)
    s[sizeof(next_section) - 2] = 0;
  Ustrcpy(next_section, s);
  return NULL;
  }

/* Return the first non-blank character. */

return s;
}