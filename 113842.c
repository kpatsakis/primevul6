onig_print_names(FILE* fp, regex_t* reg)
{
  int i, j;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t) && t->num > 0) {
    fprintf(fp, "name table\n");
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      fprintf(fp, "%s: ", e->name);
      if (e->back_num == 0) {
        fputs("-", fp);
      }
      else if (e->back_num == 1) {
        fprintf(fp, "%d", e->back_ref1);
      }
      else {
        for (j = 0; j < e->back_num; j++) {
          if (j > 0) fprintf(fp, ", ");
          fprintf(fp, "%d", e->back_refs[j]);
        }
      }
      fputs("\n", fp);
    }
    fputs("\n", fp);
  }
  return 0;
}