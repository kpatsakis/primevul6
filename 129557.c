static CPpmd_Void_Ref CutOff(CPpmd8 *p, CTX_PTR ctx, unsigned order)
{
  int i;
  unsigned tmp;
  CPpmd_State *s;
  
  if (!ctx->NumStats)
  {
    s = ONE_STATE(ctx);
    if ((Byte *)Ppmd8_GetPtr(p, SUCCESSOR(s)) >= p->UnitsStart)
    {
      if (order < p->MaxOrder)
        SetSuccessor(s, CutOff(p, CTX(SUCCESSOR(s)), order + 1));
      else
        SetSuccessor(s, 0);
      if (SUCCESSOR(s) || order <= 9) /* O_BOUND */
        return REF(ctx);
    }
    SpecialFreeUnit(p, ctx);
    return 0;
  }

  ctx->Stats = STATS_REF(MoveUnitsUp(p, STATS(ctx), tmp = ((unsigned)ctx->NumStats + 2) >> 1));

  for (s = STATS(ctx) + (i = ctx->NumStats); s >= STATS(ctx); s--)
    if ((Byte *)Ppmd8_GetPtr(p, SUCCESSOR(s)) < p->UnitsStart)
    {
      CPpmd_State *s2 = STATS(ctx) + (i--);
      SetSuccessor(s, 0);
      SwapStates(s, s2);
    }
    else if (order < p->MaxOrder)
      SetSuccessor(s, CutOff(p, CTX(SUCCESSOR(s)), order + 1));
    else
      SetSuccessor(s, 0);
    
  if (i != ctx->NumStats && order)
  {
    ctx->NumStats = (Byte)i;
    s = STATS(ctx);
    if (i < 0)
    {
      FreeUnits(p, s, tmp);
      SpecialFreeUnit(p, ctx);
      return 0;
    }
    if (i == 0)
    {
      ctx->Flags = (Byte)((ctx->Flags & 0x10) + 0x08 * (s->Symbol >= 0x40));
      *ONE_STATE(ctx) = *s;
      FreeUnits(p, s, tmp);
      /* 9.31: the code was fixed. It's was not BUG, if Freq <= MAX_FREQ = 124 */
      ONE_STATE(ctx)->Freq = (Byte)(((unsigned)ONE_STATE(ctx)->Freq + 11) >> 3);
    }
    else
      Refresh(p, ctx, tmp, ctx->SummFreq > 16 * i);
  }
  return REF(ctx);
}