scope_new(mrb_state *mrb, codegen_scope *prev, node *nlv)
{
  static const codegen_scope codegen_scope_zero = { 0 };
  mrb_pool *pool = mrb_pool_open(mrb);
  codegen_scope *s = (codegen_scope *)mrb_pool_alloc(pool, sizeof(codegen_scope));

  if (!s) {
    if (prev)
      codegen_error(prev, "unexpected scope");
    return NULL;
  }
  *s = codegen_scope_zero;
  s->mrb = mrb;
  s->mpool = pool;
  if (!prev) return s;
  s->prev = prev;
  s->ainfo = 0;
  s->mscope = 0;

  scope_add_irep(s);

  s->rcapa = 8;
  s->reps = (mrb_irep**)mrb_malloc(mrb, sizeof(mrb_irep*)*s->rcapa);

  s->icapa = 1024;
  s->iseq = (mrb_code*)mrb_malloc(mrb, sizeof(mrb_code)*s->icapa);

  s->pcapa = 32;
  s->pool = (mrb_pool_value*)mrb_malloc(mrb, sizeof(mrb_pool_value)*s->pcapa);

  s->scapa = 256;
  s->syms = (mrb_sym*)mrb_malloc(mrb, sizeof(mrb_sym)*s->scapa);

  s->lv = nlv;
  s->sp += node_len(nlv)+1;        /* add self */
  s->nlocals = s->sp;
  if (nlv) {
    mrb_sym *lv;
    node *n = nlv;
    size_t i = 0;

    s->irep->lv = lv = (mrb_sym*)mrb_malloc(mrb, sizeof(mrb_sym)*(s->nlocals-1));
    for (i=0, n=nlv; n; i++,n=n->cdr) {
      lv[i] = lv_name(n);
    }
    mrb_assert(i + 1 == s->nlocals);
  }
  s->ai = mrb_gc_arena_save(mrb);

  s->filename_sym = prev->filename_sym;
  if (s->filename_sym) {
    s->lines = (uint16_t*)mrb_malloc(mrb, sizeof(short)*s->icapa);
  }
  s->lineno = prev->lineno;

  /* debug setting */
  s->debug_start_pos = 0;
  if (s->filename_sym) {
    mrb_debug_info_alloc(mrb, s->irep);
  }
  else {
    s->irep->debug_info = NULL;
  }
  s->parser = prev->parser;
  s->filename_index = prev->filename_index;

  s->rlev = prev->rlev+1;

  return s;
}