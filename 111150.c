gen_jmpdst(codegen_scope *s, uint32_t pc)
{

  if (pc == JMPLINK_START) {
    pc = 0;
  }
  uint32_t pos2 = s->pc+2;
  int32_t off = pc - pos2;

  if (off > INT16_MAX || INT16_MIN > off) {
    codegen_error(s, "too big jump offset");
  }
  gen_S(s, (uint16_t)off);
}