static void GlueFreeBlocks(CPpmd8 *p)
{
  CPpmd8_Node_Ref head = 0;
  CPpmd8_Node_Ref *prev = &head;
  unsigned i;

  p->GlueCount = 1 << 13;
  memset(p->Stamps, 0, sizeof(p->Stamps));
  
  /* Order-0 context is always at top UNIT, so we don't need guard NODE at the end.
     All blocks up to p->LoUnit can be free, so we need guard NODE at LoUnit. */
  if (p->LoUnit != p->HiUnit)
    ((CPpmd8_Node *)p->LoUnit)->Stamp = 0;

  /* Glue free blocks */
  for (i = 0; i < PPMD_NUM_INDEXES; i++)
  {
    CPpmd8_Node_Ref next = (CPpmd8_Node_Ref)p->FreeList[i];
    p->FreeList[i] = 0;
    while (next != 0)
    {
      CPpmd8_Node *node = NODE(next);
      if (node->NU != 0)
      {
        CPpmd8_Node *node2;
        *prev = next;
        prev = &(node->Next);
        while ((node2 = node + node->NU)->Stamp == EMPTY_NODE)
        {
          node->NU += node2->NU;
          node2->NU = 0;
        }
      }
      next = node->Next;
    }
  }
  *prev = 0;
  
  /* Fill lists of free blocks */
  while (head != 0)
  {
    CPpmd8_Node *node = NODE(head);
    unsigned nu;
    head = node->Next;
    nu = node->NU;
    if (nu == 0)
      continue;
    for (; nu > 128; nu -= 128, node += 128)
      InsertNode(p, node, PPMD_NUM_INDEXES - 1);
    if (I2U(i = U2I(nu)) != nu)
    {
      unsigned k = I2U(--i);
      InsertNode(p, node + k, nu - k - 1);
    }
    InsertNode(p, node, i);
  }
}