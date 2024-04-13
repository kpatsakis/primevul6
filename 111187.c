dispatch(codegen_scope *s, uint32_t pos0)
{
  int32_t pos1;
  int32_t offset;
  int16_t newpos;

  if (pos0 == JMPLINK_START) return 0;

  pos1 = pos0 + 2;
  offset = s->pc - pos1;
  if (offset > INT16_MAX) {
    codegen_error(s, "too big jmp offset");
  }
  s->lastlabel = s->pc;
  newpos = (int16_t)PEEK_S(s->iseq+pos0);
  emit_S(s, pos0, (uint16_t)offset);
  if (newpos == 0) return 0;
  return pos1+newpos;
}