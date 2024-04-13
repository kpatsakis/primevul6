gen_binop(codegen_scope *s, mrb_sym op, uint16_t dst)
{
  if (no_peephole(s)) return FALSE;
  else if (op == MRB_OPSYM_2(s->mrb, aref)) {
    genop_1(s, OP_GETIDX, dst);
    return TRUE;
  }
  else {
    struct mrb_insn_data data = mrb_last_insn(s);
    mrb_int n, n0;
    if (addr_pc(s, data.addr) == s->lastlabel || !get_int_operand(s, &data, &n)) {
      /* not integer immediate */
      return FALSE;
    }
    struct mrb_insn_data data0 = mrb_decode_insn(mrb_prev_pc(s, data.addr));
    if (!get_int_operand(s, &data0, &n0)) {
      return FALSE;
    }
    if (op == MRB_OPSYM_2(s->mrb, lshift)) {
      if (!mrb_num_shift(s->mrb, n0, n, &n)) return FALSE;
    }
    else if (op == MRB_OPSYM_2(s->mrb, rshift)) {
      if (n == MRB_INT_MIN) return FALSE;
      if (!mrb_num_shift(s->mrb, n0, -n, &n)) return FALSE;
    }
    else if (op == MRB_OPSYM_2(s->mrb, mod) && n != 0) {
      if (n0 == MRB_INT_MIN && n == -1) {
        n = 0;
      }
      else {
        mrb_int n1 = n0 % n;
        if ((n0 < 0) != (n < 0) && n1 != 0) {
          n1 += n;
        }
        n = n1;
      }
    }
    else if (op == MRB_OPSYM_2(s->mrb, and)) {
      n = n0 & n;
    }
    else if (op == MRB_OPSYM_2(s->mrb, or)) {
      n = n0 | n;
    }
    else if (op == MRB_OPSYM_2(s->mrb, xor)) {
      n = n0 ^ n;
    }
    else {
      return FALSE;
    }
    s->pc = addr_pc(s, data0.addr);
    gen_int(s, dst, n);
    return TRUE;
  }
}