genop_2S(codegen_scope *s, mrb_code i, uint16_t a, uint16_t b)
{
  genop_1(s, i, a);
  gen_S(s, b);
}