static UInt32 GetUsedMemory(const CPpmd8 *p)
{
  UInt32 v = 0;
  unsigned i;
  for (i = 0; i < PPMD_NUM_INDEXES; i++)
    v += p->Stamps[i] * I2U(i);
  return p->Size - (UInt32)(p->HiUnit - p->LoUnit) - (UInt32)(p->UnitsStart - p->Text) - U2B(v);
}