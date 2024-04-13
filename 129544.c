void Ppmd8_Update1(CPpmd8 *p)
{
  CPpmd_State *s = p->FoundState;
  s->Freq += 4;
  p->MinContext->SummFreq += 4;
  if (s[0].Freq > s[-1].Freq)
  {
    SwapStates(&s[0], &s[-1]);
    p->FoundState = --s;
    if (s->Freq > MAX_FREQ)
      Rescale(p);
  }
  NextContext(p);
}