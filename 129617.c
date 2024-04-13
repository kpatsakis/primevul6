static void FreeUnits(CPpmd8 *p, void *ptr, unsigned nu)
{
  InsertNode(p, ptr, U2I(nu));
}