name_find(regex_t* reg, const UChar* name, const UChar* name_end)
{
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  e = (NameEntry* )NULL;
  if (IS_NOT_NULL(t)) {
    onig_st_lookup_strend(t, name, name_end, (HashDataType* )((void* )(&e)));
  }
  return e;
}