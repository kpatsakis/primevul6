static unsigned uivector_copy(uivector* p, const uivector* q)
{
  size_t i;
  if(!uivector_resize(p, q->size)) return 0;
  for(i = 0; i < q->size; i++) p->data[i] = q->data[i];
  return 1;
}