genop_0(codegen_scope *s, mrb_code i)
{
  s->lastpc = s->pc;
  gen_B(s, i);
}