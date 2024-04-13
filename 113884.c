onig_names_free(regex_t* reg)
{
  int r;
  NameTable* t;

  r = names_clear(reg);
  if (r) return r;

  t = (NameTable* )reg->name_table;
  if (IS_NOT_NULL(t)) onig_st_free_table(t);
  reg->name_table = (void* )NULL;
  return 0;
}