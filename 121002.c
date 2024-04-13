static unsigned ucvector_reserve(ucvector* p, size_t allocsize)
{
  if(allocsize > p->allocsize)
  {
    size_t newsize = (allocsize > p->allocsize * 2) ? allocsize : (allocsize * 3 / 2);
    void* data = realloc(p->data, newsize);
    if(data)
    {
      p->allocsize = newsize;
      p->data = (unsigned char*)data;
    }
    else
    {
        ucvector_cleanup(p);
        return 0; /*error: not enough memory*/
    }
  }
  return 1;
}