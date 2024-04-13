static struct per_transfer *del_per_transfer(struct per_transfer *per)
{
  struct per_transfer *n;
  struct per_transfer *p;
  DEBUGASSERT(transfers);
  DEBUGASSERT(transfersl);
  DEBUGASSERT(per);

  n = per->next;
  p = per->prev;

  if(p)
    p->next = n;
  else
    transfers = n;

  if(n)
    n->prev = p;
  else
    transfersl = p;

  free(per);

  return n;
}