void Ppmd8_Construct(CPpmd8 *p)
{
  unsigned i, k, m;

  p->Base = 0;

  for (i = 0, k = 0; i < PPMD_NUM_INDEXES; i++)
  {
    unsigned step = (i >= 12 ? 4 : (i >> 2) + 1);
    do { p->Units2Indx[k++] = (Byte)i; } while (--step);
    p->Indx2Units[i] = (Byte)k;
  }

  p->NS2BSIndx[0] = (0 << 1);
  p->NS2BSIndx[1] = (1 << 1);
  memset(p->NS2BSIndx + 2, (2 << 1), 9);
  memset(p->NS2BSIndx + 11, (3 << 1), 256 - 11);

  for (i = 0; i < 5; i++)
    p->NS2Indx[i] = (Byte)i;
  for (m = i, k = 1; i < 260; i++)
  {
    p->NS2Indx[i] = (Byte)m;
    if (--k == 0)
      k = (++m) - 4;
  }
}