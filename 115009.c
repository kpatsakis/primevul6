same_node_or_its_all_contexts_clone_p (cgraph_node *node, cgraph_node *dest)
{
  if (node == dest)
    return true;

  class ipa_node_params *info = IPA_NODE_REF (node);
  return info->is_all_contexts_clone && info->ipcp_orig_node == dest;
}