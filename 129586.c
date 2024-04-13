static UInt32 RangeDec_GetThreshold(CPpmd8 *p, UInt32 total)
{
  return p->Code / (p->Range /= total);
}