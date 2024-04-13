genop_2SS(codegen_scope *s, mrb_code i, uint16_t a, uint32_t b)
{
  genop_1(s, i, a);
  gen_S(s, b>>16);
  gen_S(s, b&0xffff);
}