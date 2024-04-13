readconf_driver_init(
  uschar *class,
  driver_instance **anchor,
  driver_info *drivers_available,
  int size_of_info,
  void *instance_default,
  int  instance_size,
  optionlist *driver_optionlist,
  int  driver_optionlist_count)
{
driver_instance **p = anchor;
driver_instance *d = NULL;
uschar *buffer;

while ((buffer = get_config_line()) != NULL)
  {
  uschar name[64];
  uschar *s;

  /* Read the first name on the line and test for the start of a new driver. A
  macro definition indicates the end of the previous driver. If this isn't the
  start of a new driver, the line will be re-read. */

  s = readconf_readname(name, sizeof(name), buffer);

  /* Handle macro definition, first finishing off the initialization of the
  previous driver, if any. */

  if (isupper(*name) && *s == '=')
    {
    if (d != NULL)
      {
      if (d->driver_name == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "no driver defined for %s \"%s\"", class, d->name);
      (d->info->init)(d);
      d = NULL;
      }
    read_macro_assignment(buffer);
    continue;
    }

  /* If the line starts with a name terminated by a colon, we are at the
  start of the definition of a new driver. The rest of the line must be
  blank. */

  if (*s++ == ':')
    {
    int i;

    /* Finish off initializing the previous driver. */

    if (d != NULL)
      {
      if (d->driver_name == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "no driver defined for %s \"%s\"", class, d->name);
      (d->info->init)(d);
      }

    /* Check that we haven't already got a driver of this name */

    for (d = *anchor; d != NULL; d = d->next)
      if (Ustrcmp(name, d->name) == 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "there are two %ss called \"%s\"", class, name);

    /* Set up a new driver instance data block on the chain, with
    its default values installed. */

    d = store_get(instance_size);
    memcpy(d, instance_default, instance_size);
    *p = d;
    p = &(d->next);
    d->name = string_copy(name);

    /* Clear out the "set" bits in the generic options */

    for (i = 0; i < driver_optionlist_count; i++)
      driver_optionlist[i].type &= ~opt_set;

    /* Check nothing more on this line, then do the next loop iteration. */

    while (isspace(*s)) s++;
    if (*s != 0) extra_chars_error(s, US"driver name ", name, US"");
    continue;
    }

  /* Not the start of a new driver. Give an error if we have not set up a
  current driver yet. */

  if (d == NULL) log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "%s name missing", class);

  /* First look to see if this is a generic option; if it is "driver",
  initialize the driver. If is it not a generic option, we can look for a
  private option provided that the driver has been previously set up. */

  if (readconf_handle_option(buffer, driver_optionlist,
        driver_optionlist_count, d, NULL))
    {
    if (d->info == NULL && d->driver_name != NULL)
      init_driver(d, drivers_available, size_of_info, class);
    }

  /* Handle private options - pass the generic block because some may
  live therein. A flag with each option indicates if it is in the public
  block. */

  else if (d->info != NULL)
    {
    readconf_handle_option(buffer, d->info->options,
      *(d->info->options_count), d, US"option \"%s\" unknown");
    }

  /* The option is not generic and the driver name has not yet been given. */

  else log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "option \"%s\" unknown "
    "(\"driver\" must be specified before any private options)", name);
  }

/* Run the initialization function for the final driver. */

if (d != NULL)
  {
  if (d->driver_name == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "no driver defined for %s \"%s\"", class, d->name);
  (d->info->init)(d);
  }
}