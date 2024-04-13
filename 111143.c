emit_S(codegen_scope *s, int pc, uint16_t i)
{
  uint8_t hi = i>>8;
  uint8_t lo = i&0xff;

  emit_B(s, pc,   hi);
  emit_B(s, pc+1, lo);
}