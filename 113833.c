onig_print_names(FILE* fp, regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    fprintf(fp, "name table\n");
    onig_st_foreach(t, i_print_name_entry, (HashDataType )fp);
    fputs("\n", fp);
  }
  return 0;
}