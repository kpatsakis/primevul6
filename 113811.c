names_clear(regex_t* reg)
{
  int i;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (IS_NOT_NULL(e->name)) {
        xfree(e->name);
        e->name       = NULL;
        e->name_len   = 0;
        e->back_num   = 0;
        e->back_alloc = 0;
        if (IS_NOT_NULL(e->back_refs)) xfree(e->back_refs);
        e->back_refs = (int* )NULL;
      }
    }
    if (IS_NOT_NULL(t->e)) {
      xfree(t->e);
      t->e = NULL;
    }
    t->num = 0;
  }
  return 0;
}