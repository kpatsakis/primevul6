static void Refresh(CPpmd8 *p, CTX_PTR ctx, unsigned oldNU, unsigned scale)
{
  unsigned i = ctx->NumStats, escFreq, sumFreq, flags;
  CPpmd_State *s = (CPpmd_State *)ShrinkUnits(p, STATS(ctx), oldNU, (i + 2) >> 1);
  ctx->Stats = REF(s);
  #ifdef PPMD8_FREEZE_SUPPORT
  /* fixed over Shkarin's code. Fixed code is not compatible with original code for some files in FREEZE mode. */
  scale |= (ctx->SummFreq >= ((UInt32)1 << 15));
  #endif
  flags = (ctx->Flags & (0x10 + 0x04 * scale)) + 0x08 * (s->Symbol >= 0x40);
  escFreq = ctx->SummFreq - s->Freq;
  sumFreq = (s->Freq = (Byte)((s->Freq + scale) >> scale));
  do
  {
    escFreq -= (++s)->Freq;
    sumFreq += (s->Freq = (Byte)((s->Freq + scale) >> scale));
    flags |= 0x08 * (s->Symbol >= 0x40);
  }
  while (--i);
  ctx->SummFreq = (UInt16)(sumFreq + ((escFreq + scale) >> scale));
  ctx->Flags = (Byte)flags;
}