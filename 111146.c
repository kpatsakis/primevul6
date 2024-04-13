gen_setxv(codegen_scope *s, uint8_t op, uint16_t dst, mrb_sym sym, int val)
{
  int idx = new_sym(s, sym);
  if (!val && !no_peephole(s)) {
    struct mrb_insn_data data = mrb_last_insn(s);
    if (data.insn == OP_MOVE && data.a == dst) {
      dst = data.b;
      rewind_pc(s);
    }
  }
  genop_2(s, op, dst, idx);
}