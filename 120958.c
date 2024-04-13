static unsigned uivector_push_back(uivector* p, unsigned c)
{
  if(!uivector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}