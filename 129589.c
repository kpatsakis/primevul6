static void NextContext(CPpmd8 *p)
{
  CTX_PTR c = CTX(SUCCESSOR(p->FoundState));
  if (p->OrderFall == 0 && (Byte *)c >= p->UnitsStart)
    p->MinContext = p->MaxContext = c;
  else
  {
    UpdateModel(p);
    p->MinContext = p->MaxContext;
  }
}