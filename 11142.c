get_set_flag(uschar *name, optionlist *oltop, int last, void *data_block)
{
optionlist *ol;
uschar name2[64];
sprintf(CS name2, "*set_%.50s", name);
ol = find_option(name2, oltop, last);
if (ol == NULL) log_write(0, LOG_MAIN|LOG_PANIC_DIE,
  "Exim internal error: missing set flag for %s", name);
return (data_block == NULL)? (BOOL *)(ol->value) :
  (BOOL *)((uschar *)data_block + (long int)(ol->value));
}