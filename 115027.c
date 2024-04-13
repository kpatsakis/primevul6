set_single_call_flag (cgraph_node *node, void *)
{
  cgraph_edge *cs = node->callers;
  /* Local thunks can be handled transparently, skip them.  */
  while (cs && cs->caller->thunk.thunk_p && cs->caller->local)
    cs = cs->next_caller;
  if (cs && IPA_NODE_REF (cs->caller))
    {
      IPA_NODE_REF (cs->caller)->node_calling_single_call = true;
      return true;
    }
  return false;
}