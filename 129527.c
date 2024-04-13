static CTX_PTR ReduceOrder(CPpmd8 *p, CPpmd_State *s1, CTX_PTR c)
{
  CPpmd_State *s = NULL;
  CTX_PTR c1 = c;
  CPpmd_Void_Ref upBranch = REF(p->Text);
  
  #ifdef PPMD8_FREEZE_SUPPORT
  /* The BUG in Shkarin's code was fixed: ps could overflow in CUT_OFF mode. */
  CPpmd_State *ps[PPMD8_MAX_ORDER + 1];
  unsigned numPs = 0;
  ps[numPs++] = p->FoundState;
  #endif

  SetSuccessor(p->FoundState, upBranch);
  p->OrderFall++;

  for (;;)
  {
    if (s1)
    {
      c = SUFFIX(c);
      s = s1;
      s1 = NULL;
    }
    else
    {
      if (!c->Suffix)
      {
        #ifdef PPMD8_FREEZE_SUPPORT
        if (p->RestoreMethod > PPMD8_RESTORE_METHOD_FREEZE)
        {
          do { SetSuccessor(ps[--numPs], REF(c)); } while (numPs);
          RESET_TEXT(1);
          p->OrderFall = 1;
        }
        #endif
        return c;
      }
      c = SUFFIX(c);
      if (c->NumStats)
      {
        if ((s = STATS(c))->Symbol != p->FoundState->Symbol)
          do { s++; } while (s->Symbol != p->FoundState->Symbol);
        if (s->Freq < MAX_FREQ - 9)
        {
          s->Freq += 2;
          c->SummFreq += 2;
        }
      }
      else
      {
        s = ONE_STATE(c);
        s->Freq = (Byte)(s->Freq + (s->Freq < 32));
      }
    }
    if (SUCCESSOR(s))
      break;
    #ifdef PPMD8_FREEZE_SUPPORT
    ps[numPs++] = s;
    #endif
    SetSuccessor(s, upBranch);
    p->OrderFall++;
  }
  
  #ifdef PPMD8_FREEZE_SUPPORT
  if (p->RestoreMethod > PPMD8_RESTORE_METHOD_FREEZE)
  {
    c = CTX(SUCCESSOR(s));
    do { SetSuccessor(ps[--numPs], REF(c)); } while (numPs);
    RESET_TEXT(1);
    p->OrderFall = 1;
    return c;
  }
  else
  #endif
  if (SUCCESSOR(s) <= upBranch)
  {
    CTX_PTR successor;
    CPpmd_State *s2 = p->FoundState;
    p->FoundState = s;

    successor = CreateSuccessors(p, False, NULL, c);
    if (successor == NULL)
      SetSuccessor(s, 0);
    else
      SetSuccessor(s, REF(successor));
    p->FoundState = s2;
  }
  
  if (p->OrderFall == 1 && c1 == p->MaxContext)
  {
    SetSuccessor(p->FoundState, SUCCESSOR(s));
    p->Text--;
  }
  if (SUCCESSOR(s) == 0)
    return NULL;
  return CTX(SUCCESSOR(s));
}