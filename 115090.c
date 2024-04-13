pop_node_from_stack (class ipa_topo_info *topo)
{
  if (topo->stack_top)
    {
      struct cgraph_node *node;
      topo->stack_top--;
      node = topo->stack[topo->stack_top];
      IPA_NODE_REF (node)->node_enqueued = 0;
      return node;
    }
  else
    return NULL;
}