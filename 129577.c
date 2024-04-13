Bool Ppmd8_RangeDec_Init(CPpmd8 *p)
{
  unsigned i;
  p->Low = 0;
  p->Range = 0xFFFFFFFF;
  p->Code = 0;
  for (i = 0; i < 4; i++)
    p->Code = (p->Code << 8) | p->Stream.In->Read(p->Stream.In);
  return (p->Code < 0xFFFFFFFF);
}