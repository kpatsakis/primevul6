bbuf_free(BBuf* bbuf)
{
  if (IS_NOT_NULL(bbuf)) {
    if (IS_NOT_NULL(bbuf->p)) xfree(bbuf->p);
    xfree(bbuf);
  }
}