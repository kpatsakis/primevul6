CPpmd_See *Ppmd8_MakeEscFreq(CPpmd8 *p, unsigned numMasked1, UInt32 *escFreq)
{
  CPpmd_See *see;
  if (p->MinContext->NumStats != 0xFF)
  {
    see = p->See[(unsigned)p->NS2Indx[(unsigned)p->MinContext->NumStats + 2] - 3] +
        (p->MinContext->SummFreq > 11 * ((unsigned)p->MinContext->NumStats + 1)) +
        2 * (unsigned)(2 * (unsigned)p->MinContext->NumStats <
        ((unsigned)SUFFIX(p->MinContext)->NumStats + numMasked1)) +
        p->MinContext->Flags;
    {
      unsigned r = (see->Summ >> see->Shift);
      see->Summ = (UInt16)(see->Summ - r);
      *escFreq = r + (r == 0);
    }
  }
  else
  {
    see = &p->DummySee;
    *escFreq = 1;
  }
  return see;
}