new_sym(codegen_scope *s, mrb_sym sym)
{
  int i, len;

  mrb_assert(s->irep);

  len = s->irep->slen;
  for (i=0; i<len; i++) {
    if (s->syms[i] == sym) return i;
  }
  if (s->irep->slen >= s->scapa) {
    s->scapa *= 2;
    if (s->scapa > 0xffff) {
      codegen_error(s, "too many symbols");
    }
    s->syms = (mrb_sym*)codegen_realloc(s, s->syms, sizeof(mrb_sym)*s->scapa);
  }
  s->syms[s->irep->slen] = sym;
  return s->irep->slen++;
}