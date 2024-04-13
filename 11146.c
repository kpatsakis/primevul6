readconf_find_option(void *p)
{
int i;
router_instance *r;
transport_instance *t;

for (i = 0; i < optionlist_config_size; i++)
  if (p == optionlist_config[i].value) return US optionlist_config[i].name;

for (r = routers; r != NULL; r = r->next)
  {
  router_info *ri = r->info;
  for (i = 0; i < ri->options_count[0]; i++)
    {
    if ((ri->options[i].type & opt_mask) != opt_stringptr) continue;
    if (p == (char *)(r->options_block) + (long int)(ri->options[i].value))
      return US ri->options[i].name;
    }
  }

for (t = transports; t != NULL; t = t->next)
  {
  transport_info *ti = t->info;
  for (i = 0; i < ti->options_count[0]; i++)
    {
    if ((ti->options[i].type & opt_mask) != opt_stringptr) continue;
    if (p == (char *)(t->options_block) + (long int)(ti->options[i].value))
      return US ti->options[i].name;
    }
  }

return US"";
}