genop_3(codegen_scope *s, mrb_code i, uint16_t a, uint16_t b, uint8_t c)
{
  genop_2(s, i, a, b);
  gen_B(s, c);
}