onig_number_of_names(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t))
    return t->num_entries;
  else
    return 0;
}