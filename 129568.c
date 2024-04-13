static void ExpandTextArea(CPpmd8 *p)
{
  UInt32 count[PPMD_NUM_INDEXES];
  unsigned i;
  memset(count, 0, sizeof(count));
  if (p->LoUnit != p->HiUnit)
    ((CPpmd8_Node *)p->LoUnit)->Stamp = 0;
  
  {
    CPpmd8_Node *node = (CPpmd8_Node *)p->UnitsStart;
    for (; node->Stamp == EMPTY_NODE; node += node->NU)
    {
      node->Stamp = 0;
      count[U2I(node->NU)]++;
    }
    p->UnitsStart = (Byte *)node;
  }
  
  for (i = 0; i < PPMD_NUM_INDEXES; i++)
  {
    CPpmd8_Node_Ref *next = (CPpmd8_Node_Ref *)&p->FreeList[i];
    while (count[i] != 0)
    {
      CPpmd8_Node *node = NODE(*next);
      while (node->Stamp == 0)
      {
        *next = node->Next;
        node = NODE(*next);
        p->Stamps[i]--;
        if (--count[i] == 0)
          break;
      }
      next = &node->Next;
    }
  }
}