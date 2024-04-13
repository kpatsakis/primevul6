static unsigned uivector_reserve(uivector* p, size_t allocsize)
{
  if(allocsize > p->allocsize)
  {
    size_t newsize = (allocsize > p->allocsize * 2) ? allocsize : (allocsize * 3 / 2);
    void* data = realloc(p->data, newsize);
    if(data)
    {
      memset(&((char*)data)[p->allocsize], 0, newsize - p->allocsize);
      p->allocsize = newsize;
      p->data = (unsigned*)data;
    }
    else
    {
        uivector_cleanup(p);
        return 0; /*error: not enough memory*/
    }
  }
  return 1;
}