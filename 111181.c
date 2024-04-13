catch_handler_new(codegen_scope *s)
{
  size_t newsize = sizeof(struct mrb_irep_catch_handler) * (s->irep->clen + 1);
  s->catch_table = (struct mrb_irep_catch_handler *)codegen_realloc(s, (void *)s->catch_table, newsize);
  return s->irep->clen ++;
}