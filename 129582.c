static void *RemoveNode(CPpmd8 *p, unsigned indx)
{
  CPpmd8_Node *node = NODE((CPpmd8_Node_Ref)p->FreeList[indx]);
  p->FreeList[indx] = node->Next;
  p->Stamps[indx]--;
  return node;
}