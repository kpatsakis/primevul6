genop_2(codegen_scope *s, mrb_code i, uint16_t a, uint16_t b)
{
  s->lastpc = s->pc;
  check_no_ext_ops(s, a, b);
  if (a > 0xff && b > 0xff) {
    gen_B(s, OP_EXT3);
    gen_B(s, i);
    gen_S(s, a);
    gen_S(s, b);
  }
  else if (b > 0xff) {
    gen_B(s, OP_EXT2);
    gen_B(s, i);
    gen_B(s, (uint8_t)a);
    gen_S(s, b);
  }
  else if (a > 0xff) {
    gen_B(s, OP_EXT1);
    gen_B(s, i);
    gen_S(s, a);
    gen_B(s, (uint8_t)b);
  }
  else {
    gen_B(s, i);
    gen_B(s, (uint8_t)a);
    gen_B(s, (uint8_t)b);
  }
}