static void RestoreModel(CPpmd8 *p, CTX_PTR c1
    #ifdef PPMD8_FREEZE_SUPPORT
    , CTX_PTR fSuccessor
    #endif
    )
{
  CTX_PTR c;
  CPpmd_State *s;
  RESET_TEXT(0);
  for (c = p->MaxContext; c != c1; c = SUFFIX(c))
    if (--(c->NumStats) == 0)
    {
      s = STATS(c);
      c->Flags = (Byte)((c->Flags & 0x10) + 0x08 * (s->Symbol >= 0x40));
      *ONE_STATE(c) = *s;
      SpecialFreeUnit(p, s);
      ONE_STATE(c)->Freq = (Byte)(((unsigned)ONE_STATE(c)->Freq + 11) >> 3);
    }
    else
      Refresh(p, c, (c->NumStats+3) >> 1, 0);
 
  for (; c != p->MinContext; c = SUFFIX(c))
    if (!c->NumStats)
      ONE_STATE(c)->Freq = (Byte)(ONE_STATE(c)->Freq - (ONE_STATE(c)->Freq >> 1));
    else if ((c->SummFreq += 4) > 128 + 4 * c->NumStats)
      Refresh(p, c, (c->NumStats + 2) >> 1, 1);

  #ifdef PPMD8_FREEZE_SUPPORT
  if (p->RestoreMethod > PPMD8_RESTORE_METHOD_FREEZE)
  {
    p->MaxContext = fSuccessor;
    p->GlueCount += !(p->Stamps[1] & 1);
  }
  else if (p->RestoreMethod == PPMD8_RESTORE_METHOD_FREEZE)
  {
    while (p->MaxContext->Suffix)
      p->MaxContext = SUFFIX(p->MaxContext);
    RemoveBinContexts(p, p->MaxContext, 0);
    p->RestoreMethod++;
    p->GlueCount = 0;
    p->OrderFall = p->MaxOrder;
  }
  else
  #endif
  if (p->RestoreMethod == PPMD8_RESTORE_METHOD_RESTART || GetUsedMemory(p) < (p->Size >> 1))
    RestartModel(p);
  else
  {
    while (p->MaxContext->Suffix)
      p->MaxContext = SUFFIX(p->MaxContext);
    do
    {
      CutOff(p, p->MaxContext, 0);
      ExpandTextArea(p);
    }
    while (GetUsedMemory(p) > 3 * (p->Size >> 2));
    p->GlueCount = 0;
    p->OrderFall = p->MaxOrder;
  }
}