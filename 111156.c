no_peephole(codegen_scope *s)
{
  return no_optimize(s) || s->lastlabel == s->pc || s->pc == 0 || s->pc == s->lastpc;
}