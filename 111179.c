gen_B(codegen_scope *s, uint8_t i)
{
  emit_B(s, s->pc, i);
  s->pc++;
}