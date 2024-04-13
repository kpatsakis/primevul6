gen_int(codegen_scope *s, uint16_t dst, mrb_int i)
{
  if (i < 0) {
    if (i == -1) genop_1(s, OP_LOADI__1, dst);
    else if (i >= -0xff) genop_2(s, OP_LOADINEG, dst, (uint16_t)-i);
    else if (i >= INT16_MIN) genop_2S(s, OP_LOADI16, dst, (uint16_t)i);
    else if (i >= INT32_MIN) genop_2SS(s, OP_LOADI32, dst, (uint32_t)i);
    else goto int_lit;
  }
  else if (i < 8) genop_1(s, OP_LOADI_0 + (uint8_t)i, dst);
  else if (i <= 0xff) genop_2(s, OP_LOADI, dst, (uint16_t)i);
  else if (i <= INT16_MAX) genop_2S(s, OP_LOADI16, dst, (uint16_t)i);
  else if (i <= INT32_MAX) genop_2SS(s, OP_LOADI32, dst, (uint32_t)i);
  else {
  int_lit:
    genop_2(s, OP_LOADL, dst, new_lit(s, mrb_int_value(s->mrb, i)));
  }
}