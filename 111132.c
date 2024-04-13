gen_return(codegen_scope *s, uint8_t op, uint16_t src)
{
  if (no_peephole(s)) {
    genop_1(s, op, src);
  }
  else {
    struct mrb_insn_data data = mrb_last_insn(s);

    if (data.insn == OP_MOVE && src == data.a) {
      rewind_pc(s);
      genop_1(s, op, data.b);
    }
    else if (data.insn != OP_RETURN) {
      genop_1(s, op, src);
    }
  }
}