push_node_to_stack (class ipa_topo_info *topo, struct cgraph_node *node)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  if (info->node_enqueued)
    return;
  info->node_enqueued = 1;
  topo->stack[topo->stack_top++] = node;
}