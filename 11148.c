init_driver(driver_instance *d, driver_info *drivers_available,
  int size_of_info, uschar *class)
{
driver_info *dd;

for (dd = drivers_available; dd->driver_name[0] != 0;
     dd = (driver_info *)(((uschar *)dd) + size_of_info))
  {
  if (Ustrcmp(d->driver_name, dd->driver_name) == 0)
    {
    int i;
    int len = dd->options_len;
    d->info = dd;
    d->options_block = store_get(len);
    memcpy(d->options_block, dd->options_block, len);
    for (i = 0; i < *(dd->options_count); i++)
      dd->options[i].type &= ~opt_set;
    return dd;
    }
  }

log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
  "%s %s: cannot find %s driver \"%s\"", class, d->name, class, d->driver_name);

return NULL;   /* never obeyed */
}