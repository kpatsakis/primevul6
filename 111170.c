codegen_palloc(codegen_scope *s, size_t len)
{
  void *p = mrb_pool_alloc(s->mpool, len);

  if (!p) codegen_error(s, "pool memory allocation");
  return p;
}