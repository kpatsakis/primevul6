readconf_rewrites(void)
{
rewrite_rule **chain = &global_rewrite_rules;
uschar *p;

while ((p = get_config_line()) != NULL)
  {
  rewrite_rule *next = readconf_one_rewrite(p, &rewrite_existflags, TRUE);
  *chain = next;
  chain = &(next->next);
  }
}