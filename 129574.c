static void *MoveUnitsUp(CPpmd8 *p, void *oldPtr, unsigned nu)
{
  unsigned indx = U2I(nu);
  void *ptr;
  if ((Byte *)oldPtr > p->UnitsStart + 16 * 1024 || REF(oldPtr) > p->FreeList[indx])
    return oldPtr;
  ptr = RemoveNode(p, indx);
  MyMem12Cpy(ptr, oldPtr, nu);
  if ((Byte*)oldPtr != p->UnitsStart)
    InsertNode(p, oldPtr, indx);
  else
    p->UnitsStart += U2B(I2U(indx));
  return ptr;
}