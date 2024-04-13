read_macro_assignment(uschar *s)
{
uschar name[64];
int namelen = 0;
BOOL redef = FALSE;
macro_item *m;
macro_item *mlast = NULL;

while (isalnum(*s) || *s == '_')
  {
  if (namelen >= sizeof(name) - 1)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
      "macro name too long (maximum is %d characters)", sizeof(name) - 1);
  name[namelen++] = *s++;
  }
name[namelen] = 0;

while (isspace(*s)) s++;
if (*s++ != '=')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "malformed macro definition");

if (*s == '=')
  {
  redef = TRUE;
  s++;
  }
while (isspace(*s)) s++;

/* If an existing macro of the same name was defined on the command line, we
just skip this definition. It's an error to attempt to redefine a macro without
redef set to TRUE, or to redefine a macro when it hasn't been defined earlier.
It is also an error to define a macro whose name begins with the name of a
previously defined macro. Note: it is documented that the other way round
works. */

for (m = macros; m != NULL; m = m->next)
  {
  int len = Ustrlen(m->name);

  if (Ustrcmp(m->name, name) == 0)
    {
    if (!m->command_line && !redef)
      log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "macro \"%s\" is already "
       "defined (use \"==\" if you want to redefine it", name);
    break;
    }

  if (len < namelen && Ustrstr(name, m->name) != NULL)
    log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "\"%s\" cannot be defined as "
      "a macro because previously defined macro \"%s\" is a substring",
      name, m->name);

  /* We cannot have this test, because it is documented that a substring
  macro is permitted (there is even an example).
  *
  * if (len > namelen && Ustrstr(m->name, name) != NULL)
  *   log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "\"%s\" cannot be defined as "
  *     "a macro because it is a substring of previously defined macro \"%s\"",
  *     name, m->name);
  */

  mlast = m;
  }

/* Check for an overriding command-line definition. */

if (m != NULL && m->command_line) return;

/* Redefinition must refer to an existing macro. */

if (redef)
  {
  if (m == NULL)
    log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "can't redefine an undefined macro "
      "\"%s\"", name);
  }

/* We have a new definition. The macro_item structure includes a final vector
called "name" which is one byte long. Thus, adding "namelen" gives us enough
room to store the "name" string. */

else
  {
  m = store_get(sizeof(macro_item) + namelen);
  if (macros == NULL) macros = m; else mlast->next = m;
  Ustrncpy(m->name, name, namelen);
  m->name[namelen] = 0;
  m->next = NULL;
  m->command_line = FALSE;
  }

/* Set the value of the new or redefined macro */

m->replacement = string_copy(s);
}