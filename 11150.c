read_named_list(tree_node **anchorp, int *numberp, int max, uschar *s,
  uschar *tname)
{
BOOL forcecache = FALSE;
uschar *ss;
tree_node *t;
namedlist_block *nb = store_get(sizeof(namedlist_block));

if (Ustrncmp(s, "_cache", 6) == 0)
  {
  forcecache = TRUE;
  s += 6;
  }

if (!isspace(*s))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "unrecognized configuration line");

if (*numberp >= max)
 log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "too many named %ss (max is %d)\n",
   tname, max);

while (isspace(*s)) s++;
ss = s;
while (isalnum(*s) || *s == '_') s++;
t = store_get(sizeof(tree_node) + s-ss);
Ustrncpy(t->name, ss, s-ss);
t->name[s-ss] = 0;
while (isspace(*s)) s++;

if (!tree_insertnode(anchorp, t))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "duplicate name \"%s\" for a named %s", t->name, tname);

t->data.ptr = nb;
nb->number = *numberp;
*numberp += 1;

if (*s++ != '=') log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
  "missing '=' after \"%s\"", t->name);
while (isspace(*s)) s++;
nb->string = read_string(s, t->name);
nb->cache_data = NULL;

/* Check the string for any expansions; if any are found, mark this list
uncacheable unless the user has explicited forced caching. */

if (!forcecache && Ustrchr(nb->string, '$') != NULL) nb->number = -1;
}