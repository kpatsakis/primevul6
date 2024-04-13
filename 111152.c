mrb_prev_pc(codegen_scope *s, const mrb_code *pc)
{
  const mrb_code *prev_pc = NULL;
  const mrb_code *i = s->iseq;

  while (i<pc) {
    uint8_t insn = i[0];
    prev_pc = i;
    switch (insn) {
    case OP_EXT1:
      i += mrb_insn_size1[i[1]] + 1;
      break;
    case OP_EXT2:
      i += mrb_insn_size2[i[1]] + 1;
      break;
    case OP_EXT3:
      i += mrb_insn_size3[i[1]] + 1;
      break;
    default:
      i += mrb_insn_size[insn];
      break;
    }
  }
  return prev_pc;
}