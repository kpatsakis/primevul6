gen_addsub(codegen_scope *s, uint8_t op, uint16_t dst)
{
  if (no_peephole(s)) {
  normal:
    genop_1(s, op, dst);
    return;
  }
  else {
    struct mrb_insn_data data = mrb_last_insn(s);
    mrb_int n;

    if (!get_int_operand(s, &data, &n)) {
      /* not integer immediate */
      goto normal;
    }
    struct mrb_insn_data data0 = mrb_decode_insn(mrb_prev_pc(s, data.addr));
    mrb_int n0;
    if (addr_pc(s, data.addr) == s->lastlabel || !get_int_operand(s, &data0, &n0)) {
      /* OP_ADDI/OP_SUBI takes upto 8bits */
      if (n > INT8_MAX || n < INT8_MIN) goto normal;
      rewind_pc(s);
      if (n == 0) return;
      if (n > 0) {
        if (op == OP_ADD) genop_2(s, OP_ADDI, dst, (uint16_t)n);
        else genop_2(s, OP_SUBI, dst, (uint16_t)n);
      }
      else {                    /* n < 0 */
        n = -n;
        if (op == OP_ADD) genop_2(s, OP_SUBI, dst, (uint16_t)n);
        else genop_2(s, OP_ADDI, dst, (uint16_t)n);
      }
      return;
    }
    if (op == OP_ADD) {
      if (mrb_int_add_overflow(n0, n, &n)) goto normal;
    }
    else { /* OP_SUB */
      if (mrb_int_sub_overflow(n0, n, &n)) goto normal;
    }
    s->pc = addr_pc(s, data0.addr);
    gen_int(s, dst, n);
  }
}