i_print_name_entry(UChar* key, NameEntry* e, void* arg)
{
  int i;
  FILE* fp = (FILE* )arg;

  fprintf(fp, "%s: ", e->name);
  if (e->back_num == 0)
    fputs("-", fp);
  else if (e->back_num == 1)
    fprintf(fp, "%d", e->back_ref1);
  else {
    for (i = 0; i < e->back_num; i++) {
      if (i > 0) fprintf(fp, ", ");
      fprintf(fp, "%d", e->back_refs[i]);
    }
  }
  fputs("\n", fp);
  return ST_CONTINUE;
}