gen_getupvar(codegen_scope *s, uint16_t dst, mrb_sym id)
{
  int idx;
  int lv = search_upvar(s, id, &idx);

  if (!no_peephole(s)) {
    struct mrb_insn_data data = mrb_last_insn(s);
    if (data.insn == OP_SETUPVAR && data.a == dst && data.b == idx && data.c == lv) {
      /* skip GETUPVAR right after SETUPVAR */
      return;
    }
  }
  genop_3(s, OP_GETUPVAR, dst, idx, lv);
}