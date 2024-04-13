static CTX_PTR CreateSuccessors(CPpmd8 *p, Bool skip, CPpmd_State *s1, CTX_PTR c)
{
  CPpmd_State upState;
  Byte flags;
  CPpmd_Byte_Ref upBranch = (CPpmd_Byte_Ref)SUCCESSOR(p->FoundState);
  /* fixed over Shkarin's code. Maybe it could work without + 1 too. */
  CPpmd_State *ps[PPMD8_MAX_ORDER + 1];
  unsigned numPs = 0;
  
  if (!skip)
    ps[numPs++] = p->FoundState;
  
  while (c->Suffix)
  {
    CPpmd_Void_Ref successor;
    CPpmd_State *s;
    c = SUFFIX(c);
    if (s1)
    {
      s = s1;
      s1 = NULL;
    }
    else if (c->NumStats != 0)
    {
      for (s = STATS(c); s->Symbol != p->FoundState->Symbol; s++);
      if (s->Freq < MAX_FREQ - 9)
      {
        s->Freq++;
        c->SummFreq++;
      }
    }
    else
    {
      s = ONE_STATE(c);
      s->Freq = (Byte)(s->Freq + (!SUFFIX(c)->NumStats & (s->Freq < 24)));
    }
    successor = SUCCESSOR(s);
    if (successor != upBranch)
    {
      c = CTX(successor);
      if (numPs == 0)
        return c;
      break;
    }
    ps[numPs++] = s;
  }
  
  upState.Symbol = *(const Byte *)Ppmd8_GetPtr(p, upBranch);
  SetSuccessor(&upState, upBranch + 1);
  flags = (Byte)(0x10 * (p->FoundState->Symbol >= 0x40) + 0x08 * (upState.Symbol >= 0x40));

  if (c->NumStats == 0)
    upState.Freq = ONE_STATE(c)->Freq;
  else
  {
    UInt32 cf, s0;
    CPpmd_State *s;
    for (s = STATS(c); s->Symbol != upState.Symbol; s++);
    cf = s->Freq - 1;
    s0 = c->SummFreq - c->NumStats - cf;
    upState.Freq = (Byte)(1 + ((2 * cf <= s0) ? (5 * cf > s0) : ((cf + 2 * s0 - 3) / s0)));
  }

  do
  {
    /* Create Child */
    CTX_PTR c1; /* = AllocContext(p); */
    if (p->HiUnit != p->LoUnit)
      c1 = (CTX_PTR)(p->HiUnit -= UNIT_SIZE);
    else if (p->FreeList[0] != 0)
      c1 = (CTX_PTR)RemoveNode(p, 0);
    else
    {
      c1 = (CTX_PTR)AllocUnitsRare(p, 0);
      if (!c1)
        return NULL;
    }
    c1->NumStats = 0;
    c1->Flags = flags;
    *ONE_STATE(c1) = upState;
    c1->Suffix = REF(c);
    SetSuccessor(ps[--numPs], REF(c1));
    c = c1;
  }
  while (numPs != 0);
  
  return c;
}