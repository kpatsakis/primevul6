raise_error(codegen_scope *s, const char *msg)
{
  int idx = new_lit(s, mrb_str_new_cstr(s->mrb, msg));

  genop_1(s, OP_ERR, idx);
}