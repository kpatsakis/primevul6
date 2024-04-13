attrsym(codegen_scope *s, mrb_sym a)
{
  const char *name;
  mrb_int len;
  char *name2;

  name = mrb_sym_name_len(s->mrb, a, &len);
  name2 = (char *)codegen_palloc(s,
                                 (size_t)len
                                 + 1 /* '=' */
                                 + 1 /* '\0' */
                                 );
  mrb_assert_int_fit(mrb_int, len, size_t, SIZE_MAX);
  memcpy(name2, name, (size_t)len);
  name2[len] = '=';
  name2[len+1] = '\0';

  return mrb_intern(s->mrb, name2, len+1);
}