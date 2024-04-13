gen_S(codegen_scope *s, uint16_t i)
{
  emit_S(s, s->pc, i);
  s->pc += 2;
}