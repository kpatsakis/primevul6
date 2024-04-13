static void UpdateModel(CPpmd8 *p)
{
  CPpmd_Void_Ref successor, fSuccessor = SUCCESSOR(p->FoundState);
  CTX_PTR c;
  unsigned s0, ns, fFreq = p->FoundState->Freq;
  Byte flag, fSymbol = p->FoundState->Symbol;
  CPpmd_State *s = NULL;
  
  if (p->FoundState->Freq < MAX_FREQ / 4 && p->MinContext->Suffix != 0)
  {
    c = SUFFIX(p->MinContext);
    
    if (c->NumStats == 0)
    {
      s = ONE_STATE(c);
      if (s->Freq < 32)
        s->Freq++;
    }
    else
    {
      s = STATS(c);
      if (s->Symbol != p->FoundState->Symbol)
      {
        do { s++; } while (s->Symbol != p->FoundState->Symbol);
        if (s[0].Freq >= s[-1].Freq)
        {
          SwapStates(&s[0], &s[-1]);
          s--;
        }
      }
      if (s->Freq < MAX_FREQ - 9)
      {
        s->Freq += 2;
        c->SummFreq += 2;
      }
    }
  }
  
  c = p->MaxContext;
  if (p->OrderFall == 0 && fSuccessor)
  {
    CTX_PTR cs = CreateSuccessors(p, True, s, p->MinContext);
    if (cs == 0)
    {
      SetSuccessor(p->FoundState, 0);
      RESTORE_MODEL(c, CTX(fSuccessor));
    }
    else
    {
      SetSuccessor(p->FoundState, REF(cs));
      p->MaxContext = cs;
    }
    return;
  }
  
  *p->Text++ = p->FoundState->Symbol;
  successor = REF(p->Text);
  if (p->Text >= p->UnitsStart)
  {
    RESTORE_MODEL(c, CTX(fSuccessor)); /* check it */
    return;
  }
  
  if (!fSuccessor)
  {
    CTX_PTR cs = ReduceOrder(p, s, p->MinContext);
    if (cs == NULL)
    {
      RESTORE_MODEL(c, 0);
      return;
    }
    fSuccessor = REF(cs);
  }
  else if ((Byte *)Ppmd8_GetPtr(p, fSuccessor) < p->UnitsStart)
  {
    CTX_PTR cs = CreateSuccessors(p, False, s, p->MinContext);
    if (cs == NULL)
    {
      RESTORE_MODEL(c, 0);
      return;
    }
    fSuccessor = REF(cs);
  }
  
  if (--p->OrderFall == 0)
  {
    successor = fSuccessor;
    p->Text -= (p->MaxContext != p->MinContext);
  }
  #ifdef PPMD8_FREEZE_SUPPORT
  else if (p->RestoreMethod > PPMD8_RESTORE_METHOD_FREEZE)
  {
    successor = fSuccessor;
    RESET_TEXT(0);
    p->OrderFall = 0;
  }
  #endif
  
  s0 = p->MinContext->SummFreq - (ns = p->MinContext->NumStats) - fFreq;
  flag = (Byte)(0x08 * (fSymbol >= 0x40));
  
  for (; c != p->MinContext; c = SUFFIX(c))
  {
    unsigned ns1;
    UInt32 cf, sf;
    if ((ns1 = c->NumStats) != 0)
    {
      if ((ns1 & 1) != 0)
      {
        /* Expand for one UNIT */
        unsigned oldNU = (ns1 + 1) >> 1;
        unsigned i = U2I(oldNU);
        if (i != U2I(oldNU + 1))
        {
          void *ptr = AllocUnits(p, i + 1);
          void *oldPtr;
          if (!ptr)
          {
            RESTORE_MODEL(c, CTX(fSuccessor));
            return;
          }
          oldPtr = STATS(c);
          MyMem12Cpy(ptr, oldPtr, oldNU);
          InsertNode(p, oldPtr, i);
          c->Stats = STATS_REF(ptr);
        }
      }
      c->SummFreq = (UInt16)(c->SummFreq + (3 * ns1 + 1 < ns));
    }
    else
    {
      CPpmd_State *s2 = (CPpmd_State*)AllocUnits(p, 0);
      if (!s2)
      {
        RESTORE_MODEL(c, CTX(fSuccessor));
        return;
      }
      *s2 = *ONE_STATE(c);
      c->Stats = REF(s2);
      if (s2->Freq < MAX_FREQ / 4 - 1)
        s2->Freq <<= 1;
      else
        s2->Freq = MAX_FREQ - 4;
      c->SummFreq = (UInt16)(s2->Freq + p->InitEsc + (ns > 2));
    }
    cf = 2 * fFreq * (c->SummFreq + 6);
    sf = (UInt32)s0 + c->SummFreq;
    if (cf < 6 * sf)
    {
      cf = 1 + (cf > sf) + (cf >= 4 * sf);
      c->SummFreq += 4;
    }
    else
    {
      cf = 4 + (cf > 9 * sf) + (cf > 12 * sf) + (cf > 15 * sf);
      c->SummFreq = (UInt16)(c->SummFreq + cf);
    }
    {
      CPpmd_State *s2 = STATS(c) + ns1 + 1;
      SetSuccessor(s2, successor);
      s2->Symbol = fSymbol;
      s2->Freq = (Byte)cf;
      c->Flags |= flag;
      c->NumStats = (Byte)(ns1 + 1);
    }
  }
  p->MaxContext = p->MinContext = CTX(fSuccessor);
}