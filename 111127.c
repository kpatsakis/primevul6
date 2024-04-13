gen_setupvar(codegen_scope *s, uint16_t dst, mrb_sym id)
{
  int idx;
  int lv = search_upvar(s, id, &idx);

  if (!no_peephole(s)) {
    struct mrb_insn_data data = mrb_last_insn(s);
    if (data.insn == OP_MOVE && data.a == dst) {
      dst = data.b;
      rewind_pc(s);
    }
  }
  genop_3(s, OP_SETUPVAR, dst, idx, lv);
}