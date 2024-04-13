write_primary_da(void)
{
  string primary_da = primary_da4;
  char * vt = strstr(cfg.term, "vt");
  if (vt) {
    unsigned int ver;
    if (sscanf(vt + 2, "%u", &ver) == 1) {
      if (ver >= 500)
        primary_da = primary_da5;
      else if (ver >= 400)
        primary_da = primary_da4;
      else if (ver >= 300)
        primary_da = primary_da3;
      else if (ver >= 200)
        primary_da = primary_da2;
      else
        primary_da = primary_da1;
    }
  }
  child_write(primary_da, strlen(primary_da));
}