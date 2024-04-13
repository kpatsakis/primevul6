gen_uniop(codegen_scope *s, mrb_sym sym, uint16_t dst)
{
  if (no_peephole(s)) return FALSE;
  struct mrb_insn_data data = mrb_last_insn(s);
  mrb_int n;

  if (!get_int_operand(s, &data, &n)) return FALSE;
  if (sym == MRB_OPSYM_2(s->mrb, plus)) {
    /* unary plus does nothing */
  }
  else if (sym == MRB_OPSYM_2(s->mrb, minus)) {
    if (n == MRB_INT_MIN) return FALSE;
    n = -n;
  }
  else if (sym == MRB_OPSYM_2(s->mrb, neg)) {
    n = ~n;
  }
  else {
    return FALSE;
  }
  s->pc = addr_pc(s, data.addr);
  gen_int(s, dst, n);
  return TRUE;
}