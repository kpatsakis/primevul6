onig_foreach_name(regex_t* reg,
  int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*), void* arg)
{
  INamesArg narg;
  NameTable* t = (NameTable* )reg->name_table;

  narg.ret = 0;
  if (IS_NOT_NULL(t)) {
    narg.func = func;
    narg.reg  = reg;
    narg.arg  = arg;
    narg.enc  = reg->enc; /* should be pattern encoding. */
    onig_st_foreach(t, i_names, (HashDataType )&narg);
  }
  return narg.ret;
}