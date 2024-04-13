print_ol(optionlist *ol, uschar *name, void *options_block,
  optionlist *oltop, int last)
{
struct passwd *pw;
struct group *gr;
optionlist *ol2;
void *value;
uid_t *uidlist;
gid_t *gidlist;
uschar *s;
uschar name2[64];

if (ol == NULL)
  {
  printf("%s is not a known option\n", name);
  return;
  }

/* Non-admin callers cannot see options that have been flagged secure by the
"hide" prefix. */

if (!admin_user && (ol->type & opt_secure) != 0)
  {
  printf("%s = <value not displayable>\n", name);
  return;
  }

/* Else show the value of the option */

value = ol->value;
if (options_block != NULL)
  {
  if ((ol->type & opt_public) == 0)
    options_block = (void *)(((driver_instance *)options_block)->options_block);
  value = (void *)((uschar *)options_block + (long int)value);
  }

switch(ol->type & opt_mask)
  {
  case opt_stringptr:
  case opt_rewrite:        /* Show the text value */
  s = *((uschar **)value);
  printf("%s = %s\n", name, (s == NULL)? US"" : string_printing2(s, FALSE));
  break;

  case opt_int:
  printf("%s = %d\n", name, *((int *)value));
  break;

  case opt_mkint:
    {
    int x = *((int *)value);
    if (x != 0 && (x & 1023) == 0)
      {
      int c = 'K';
      x >>= 10;
      if ((x & 1023) == 0)
        {
        c = 'M';
        x >>= 10;
        }
      printf("%s = %d%c\n", name, x, c);
      }
    else printf("%s = %d\n", name, x);
    }
  break;

  case opt_Kint:
    {
    int x = *((int *)value);
    if (x == 0) printf("%s = 0\n", name);
      else if ((x & 1023) == 0) printf("%s = %dM\n", name, x >> 10);
        else printf("%s = %dK\n", name, x);
    }
  break;

  case opt_octint:
  printf("%s = %#o\n", name, *((int *)value));
  break;

  /* Can be negative only when "unset", in which case integer */

  case opt_fixed:
    {
    int x = *((int *)value);
    int f = x % 1000;
    int d = 100;
    if (x < 0) printf("%s =\n", name); else
      {
      printf("%s = %d.", name, x/1000);
      do
        {
        printf("%d", f/d);
        f %= d;
        d /= 10;
        }
      while (f != 0);
      printf("\n");
      }
    }
  break;

  /* If the numerical value is unset, try for the string value */

  case opt_expand_uid:
  if (! *get_set_flag(name, oltop, last, options_block))
    {
    sprintf(CS name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      void *value2 = ol2->value;
      if (options_block != NULL)
        value2 = (void *)((uschar *)options_block + (long int)value2);
      s = *((uschar **)value2);
      printf("%s = %s\n", name, (s == NULL)? US"" : string_printing(s));
      break;
      }
    }

  /* Else fall through */

  case opt_uid:
  if (! *get_set_flag(name, oltop, last, options_block))
    printf("%s =\n", name);
  else
    {
    pw = getpwuid(*((uid_t *)value));
    if (pw == NULL)
      printf("%s = %ld\n", name, (long int)(*((uid_t *)value)));
    else printf("%s = %s\n", name, pw->pw_name);
    }
  break;

  /* If the numerical value is unset, try for the string value */

  case opt_expand_gid:
  if (! *get_set_flag(name, oltop, last, options_block))
    {
    sprintf(CS name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL && (ol2->type & opt_mask) == opt_stringptr)
      {
      void *value2 = ol2->value;
      if (options_block != NULL)
        value2 = (void *)((uschar *)options_block + (long int)value2);
      s = *((uschar **)value2);
      printf("%s = %s\n", name, (s == NULL)? US"" : string_printing(s));
      break;
      }
    }

  /* Else fall through */

  case opt_gid:
  if (! *get_set_flag(name, oltop, last, options_block))
    printf("%s =\n", name);
  else
    {
    gr = getgrgid(*((int *)value));
    if (gr == NULL)
       printf("%s = %ld\n", name, (long int)(*((int *)value)));
    else printf("%s = %s\n", name, gr->gr_name);
    }
  break;

  case opt_uidlist:
  uidlist = *((uid_t **)value);
  printf("%s =", name);
  if (uidlist != NULL)
    {
    int i;
    uschar sep = ' ';
    for (i = 1; i <= (int)(uidlist[0]); i++)
      {
      uschar *name = NULL;
      pw = getpwuid(uidlist[i]);
      if (pw != NULL) name = US pw->pw_name;
      if (name != NULL) printf("%c%s", sep, name);
        else printf("%c%ld", sep, (long int)(uidlist[i]));
      sep = ':';
      }
    }
  printf("\n");
  break;

  case opt_gidlist:
  gidlist = *((gid_t **)value);
  printf("%s =", name);
  if (gidlist != NULL)
    {
    int i;
    uschar sep = ' ';
    for (i = 1; i <= (int)(gidlist[0]); i++)
      {
      uschar *name = NULL;
      gr = getgrgid(gidlist[i]);
      if (gr != NULL) name = US gr->gr_name;
      if (name != NULL) printf("%c%s", sep, name);
        else printf("%c%ld", sep, (long int)(gidlist[i]));
      sep = ':';
      }
    }
  printf("\n");
  break;

  case opt_time:
  printf("%s = %s\n", name, readconf_printtime(*((int *)value)));
  break;

  case opt_timelist:
    {
    int i;
    int *list = (int *)value;
    printf("%s = ", name);
    for (i = 0; i < list[1]; i++)
      printf("%s%s", (i == 0)? "" : ":", readconf_printtime(list[i+2]));
    printf("\n");
    }
  break;

  case opt_bit:
  printf("%s%s\n", ((*((int *)value)) & (1 << ((ol->type >> 16) & 31)))?
    "" : "no_", name);
  break;

  case opt_expand_bool:
  sprintf(CS name2, "*expand_%.50s", name);
  ol2 = find_option(name2, oltop, last);
  if (ol2 != NULL && ol2->value != NULL)
    {
    void *value2 = ol2->value;
    if (options_block != NULL)
      value2 = (void *)((uschar *)options_block + (long int)value2);
    s = *((uschar **)value2);
    if (s != NULL)
      {
      printf("%s = %s\n", name, string_printing(s));
      break;
      }
    /* s == NULL => string not set; fall through */
    }

  /* Fall through */

  case opt_bool:
  case opt_bool_verify:
  case opt_bool_set:
  printf("%s%s\n", (*((BOOL *)value))? "" : "no_", name);
  break;
  }
}