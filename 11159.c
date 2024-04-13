auths_init(void)
{
auth_instance *au, *bu;
readconf_driver_init(US"authenticator",
  (driver_instance **)(&auths),      /* chain anchor */
  (driver_info *)auths_available,    /* available drivers */
  sizeof(auth_info),                 /* size of info block */
  &auth_defaults,                    /* default values for generic options */
  sizeof(auth_instance),             /* size of instance block */
  optionlist_auths,                  /* generic options */
  optionlist_auths_size);

for (au = auths; au != NULL; au = au->next)
  {
  if (au->public_name == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "no public name specified for "
      "the %s authenticator", au->name);
  for (bu = au->next; bu != NULL; bu = bu->next)
    {
    if (strcmpic(au->public_name, bu->public_name) == 0)
      {
      if ((au->client && bu->client) || (au->server && bu->server))
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "two %s authenticators "
          "(%s and %s) have the same public name (%s)",
          (au->client)? US"client" : US"server", au->name, bu->name,
          au->public_name);
      }
    }
  }
}