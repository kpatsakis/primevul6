static CPpmd_Void_Ref RemoveBinContexts(CPpmd8 *p, CTX_PTR ctx, unsigned order)
{
  CPpmd_State *s;
  if (!ctx->NumStats)
  {
    s = ONE_STATE(ctx);
    if ((Byte *)Ppmd8_GetPtr(p, SUCCESSOR(s)) >= p->UnitsStart && order < p->MaxOrder)
      SetSuccessor(s, RemoveBinContexts(p, CTX(SUCCESSOR(s)), order + 1));
    else
      SetSuccessor(s, 0);
    /* Suffix context can be removed already, since different (high-order)
       Successors may refer to same context. So we check Flags == 0xFF (Stamp == EMPTY_NODE) */
    if (!SUCCESSOR(s) && (!SUFFIX(ctx)->NumStats || SUFFIX(ctx)->Flags == 0xFF))
    {
      FreeUnits(p, ctx, 1);
      return 0;
    }
    else
      return REF(ctx);
  }

  for (s = STATS(ctx) + ctx->NumStats; s >= STATS(ctx); s--)
    if ((Byte *)Ppmd8_GetPtr(p, SUCCESSOR(s)) >= p->UnitsStart && order < p->MaxOrder)
      SetSuccessor(s, RemoveBinContexts(p, CTX(SUCCESSOR(s)), order + 1));
    else
      SetSuccessor(s, 0);
  
  return REF(ctx);
}