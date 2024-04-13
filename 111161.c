gen_intern(codegen_scope *s)
{
  pop();
  if (!no_peephole(s)) {
    struct mrb_insn_data data = mrb_last_insn(s);

    if (data.insn == OP_STRING && data.a == cursp()) {
      rewind_pc(s);
      genop_2(s, OP_SYMBOL, data.a, data.b);
      push();
      return;
    }
  }
  genop_1(s, OP_INTERN, cursp());
  push();
}