bbuf_clone(BBuf** rto, BBuf* from)
{
  int r;
  BBuf *to;

  *rto = to = (BBuf* )xmalloc(sizeof(BBuf));
  CHECK_NULL_RETURN_MEMERR(to);
  r = BBUF_INIT(to, from->alloc);
  if (r != 0) return r;
  to->used = from->used;
  xmemcpy(to->p, from->p, from->used);
  return 0;
}