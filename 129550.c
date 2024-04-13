void Ppmd8_Free(CPpmd8 *p)
{
  free(p->Base);
  p->Size = 0;
  p->Base = 0;
}