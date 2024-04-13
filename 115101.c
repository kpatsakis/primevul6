ipcp_versionable_function_p (struct cgraph_node *node)
{
  return IPA_NODE_REF (node) && IPA_NODE_REF (node)->versionable;
}