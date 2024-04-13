scope_add_irep(codegen_scope *s)
{
  mrb_irep *irep;
  codegen_scope *prev = s->prev;

  if (prev->irep == NULL) {
    irep = mrb_add_irep(s->mrb);
    prev->irep = s->irep = irep;
    return;
  }
  else {
    if (prev->irep->rlen == UINT16_MAX) {
      codegen_error(s, "too many nested blocks/methods");
    }
    s->irep = irep = mrb_add_irep(s->mrb);
    if (prev->irep->rlen == prev->rcapa) {
      prev->rcapa *= 2;
      prev->reps = (mrb_irep**)codegen_realloc(s, prev->reps, sizeof(mrb_irep*)*prev->rcapa);
    }
    prev->reps[prev->irep->rlen] = irep;
    prev->irep->rlen++;
  }
}