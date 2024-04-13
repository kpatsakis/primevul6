static void SpecialFreeUnit(CPpmd8 *p, void *ptr)
{
  if ((Byte *)ptr != p->UnitsStart)
    InsertNode(p, ptr, 0);
  else
  {
    #ifdef PPMD8_FREEZE_SUPPORT
    *(UInt32 *)ptr = EMPTY_NODE; /* it's used for (Flags == 0xFF) check in RemoveBinContexts */
    #endif
    p->UnitsStart += UNIT_SIZE;
  }
}