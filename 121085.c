static unsigned ucvector_push_back(ucvector* p, unsigned char c)
{
  if(!ucvector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}