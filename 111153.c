generate_code(mrb_state *mrb, parser_state *p, int val)
{
  codegen_scope *scope = scope_new(mrb, 0, 0);
  struct mrb_jmpbuf *prev_jmp = mrb->jmp;
  struct mrb_jmpbuf jmpbuf;
  struct RProc *proc;

  mrb->jmp = &jmpbuf;

  scope->mrb = mrb;
  scope->parser = p;
  scope->filename_sym = p->filename_sym;
  scope->filename_index = p->current_filename_index;

  MRB_TRY(mrb->jmp) {
    /* prepare irep */
    codegen(scope, p->tree, val);
    proc = mrb_proc_new(mrb, scope->irep);
    mrb_irep_decref(mrb, scope->irep);
    mrb_pool_close(scope->mpool);
    proc->c = NULL;
    if (mrb->c->cibase && mrb->c->cibase->proc == proc->upper) {
      proc->upper = NULL;
    }
    mrb->jmp = prev_jmp;
    return proc;
  }
  MRB_CATCH(mrb->jmp) {
    mrb_irep_decref(mrb, scope->irep);
    mrb_pool_close(scope->mpool);
    mrb->jmp = prev_jmp;
    return NULL;
  }
  MRB_END_EXC(mrb->jmp);
}