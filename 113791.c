onig_renumber_name_table(regex_t* reg, GroupNumRemap* map)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_renumber_name, (HashDataType )map);
  }
  return 0;
}