local_scan_init(void)
{
#ifndef LOCAL_SCAN_HAS_OPTIONS
log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN, "local_scan() options not supported: "
  "(LOCAL_SCAN_HAS_OPTIONS not defined in Local/Makefile)");
#else

uschar *p;
while ((p = get_config_line()) != NULL)
  {
  (void) readconf_handle_option(p, local_scan_options, local_scan_options_count,
    NULL, US"local_scan option \"%s\" unknown");
  }
#endif
}