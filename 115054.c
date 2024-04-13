cgraph_edge_brings_all_scalars_for_node (struct cgraph_edge *cs,
					 struct cgraph_node *node)
{
  class ipa_node_params *dest_info = IPA_NODE_REF (node);
  int count = ipa_get_param_count (dest_info);
  class ipa_node_params *caller_info;
  class ipa_edge_args *args;
  int i;

  caller_info = IPA_NODE_REF (cs->caller);
  args = IPA_EDGE_REF (cs);
  for (i = 0; i < count; i++)
    {
      struct ipa_jump_func *jump_func;
      tree val, t;

      val = dest_info->known_csts[i];
      if (!val)
	continue;

      if (i >= ipa_get_cs_argument_count (args))
	return false;
      jump_func = ipa_get_ith_jump_func (args, i);
      t = ipa_value_from_jfunc (caller_info, jump_func,
				ipa_get_type (dest_info, i));
      if (!t || !values_equal_for_ipcp_p (val, t))
	return false;
    }
  return true;
}