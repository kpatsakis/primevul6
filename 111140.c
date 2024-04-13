search_upvar(codegen_scope *s, mrb_sym id, int *idx)
{
  const struct RProc *u;
  int lv = 0;
  codegen_scope *up = s->prev;

  while (up) {
    *idx = lv_idx(up, id);
    if (*idx > 0) {
      return lv;
    }
    lv ++;
    up = up->prev;
  }

  if (lv < 1) lv = 1;
  u = s->parser->upper;
  while (u && !MRB_PROC_CFUNC_P(u)) {
    const struct mrb_irep *ir = u->body.irep;
    uint_fast16_t n = ir->nlocals;
    int i;

    const mrb_sym *v = ir->lv;
    if (v) {
      for (i=1; n > 1; n--, v++, i++) {
        if (*v == id) {
          *idx = i;
          return lv - 1;
        }
      }
    }
    if (MRB_PROC_SCOPE_P(u)) break;
    u = u->upper;
    lv ++;
  }

  codegen_error(s, "Can't found local variables");
  return -1; /* not reached */
}