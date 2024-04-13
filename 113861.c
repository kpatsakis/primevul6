names_clear(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_name_entry, 0);
  }
  return 0;
}