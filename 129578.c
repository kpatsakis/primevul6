void Ppmd8_Update1_0(CPpmd8 *p)
{
  p->PrevSuccess = (2 * p->FoundState->Freq >= p->MinContext->SummFreq);
  p->RunLength += p->PrevSuccess;
  p->MinContext->SummFreq += 4;
  if ((p->FoundState->Freq += 4) > MAX_FREQ)
    Rescale(p);
  NextContext(p);
}