codegen_error(codegen_scope *s, const char *message)
{
  if (!s) return;
#ifndef MRB_NO_STDIO
  if (s->filename_sym && s->lineno) {
    const char *filename = mrb_sym_name_len(s->mrb, s->filename_sym, NULL);
    fprintf(stderr, "%s:%d: %s\n", filename, s->lineno, message);
  }
  else {
    fprintf(stderr, "%s\n", message);
  }
#endif
  while (s->prev) {
    codegen_scope *tmp = s->prev;
    if (s->irep) {
      mrb_free(s->mrb, s->iseq);
      for (int i=0; i<s->irep->plen; i++) {
        mrb_pool_value *pv = &s->pool[i];
        if ((pv->tt & 0x3) == IREP_TT_STR || pv->tt == IREP_TT_BIGINT) {
          mrb_free(s->mrb, (void*)pv->u.str);
        }
      }
      mrb_free(s->mrb, s->pool);
      mrb_free(s->mrb, s->syms);
      mrb_free(s->mrb, s->catch_table);
      if (s->reps) {
        /* copied from mrb_irep_free() in state.c */
        for (int i=0; i<s->irep->rlen; i++) {
          if (s->reps[i])
            mrb_irep_decref(s->mrb, (mrb_irep*)s->reps[i]);
        }
        mrb_free(s->mrb, s->reps);
      }
      mrb_free(s->mrb, s->lines);
    }
    mrb_pool_close(s->mpool);
    s = tmp;
  }
  MRB_THROW(s->mrb->jmp);
}