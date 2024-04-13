lv_idx(codegen_scope *s, mrb_sym id)
{
  node *lv = s->lv;
  int n = 1;

  while (lv) {
    if (lv_name(lv) == id) return n;
    n++;
    lv = lv->cdr;
  }
  return 0;
}