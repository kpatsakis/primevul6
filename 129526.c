Bool Ppmd8_Alloc(CPpmd8 *p, UInt32 size)
{
  if (p->Base == 0 || p->Size != size)
  {
    Ppmd8_Free(p);
    p->AlignOffset =
      #ifdef PPMD_32BIT
        (4 - size) & 3;
      #else
        4 - (size & 3);
      #endif
    if ((p->Base = (Byte *)malloc(p->AlignOffset + size)) == 0)
      return False;
    p->Size = size;
  }
  return True;
}