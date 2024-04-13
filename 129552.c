static void *ShrinkUnits(CPpmd8 *p, void *oldPtr, unsigned oldNU, unsigned newNU)
{
  unsigned i0 = U2I(oldNU);
  unsigned i1 = U2I(newNU);
  if (i0 == i1)
    return oldPtr;
  if (p->FreeList[i1] != 0)
  {
    void *ptr = RemoveNode(p, i1);
    MyMem12Cpy(ptr, oldPtr, newNU);
    InsertNode(p, oldPtr, i0);
    return ptr;
  }
  SplitBlock(p, oldPtr, i0, i1);
  return oldPtr;
}