void Ppmd8_UpdateBin(CPpmd8 *p)
{
  p->FoundState->Freq = (Byte)(p->FoundState->Freq + (p->FoundState->Freq < 196));
  p->PrevSuccess = 1;
  p->RunLength++;
  NextContext(p);
}