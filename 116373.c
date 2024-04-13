write_bell(void)
{
  if (cfg.bell_flash)
    term_schedule_vbell(false, 0);
  win_bell(&cfg);
}