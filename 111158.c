mrb_irep_remove_lv(mrb_state *mrb, mrb_irep *irep)
{
  int i;

  if (irep->flags & MRB_IREP_NO_FREE) return;
  if (irep->lv) {
    mrb_free(mrb, (void*)irep->lv);
    irep->lv = NULL;
  }
  if (!irep->reps) return;
  for (i = 0; i < irep->rlen; ++i) {
    mrb_irep_remove_lv(mrb, (mrb_irep*)irep->reps[i]);
  }
}