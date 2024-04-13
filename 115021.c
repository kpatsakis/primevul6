cgraph_edge_brings_value_p (cgraph_edge *cs,
			    ipcp_value_source<ipa_polymorphic_call_context> *src,
			    cgraph_node *dest,
			    ipcp_value<ipa_polymorphic_call_context> *)
{
  class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
  enum availability avail;
  cgraph_node *real_dest = cs->callee->function_symbol (&avail);

  if (avail <= AVAIL_INTERPOSABLE
      || !same_node_or_its_all_contexts_clone_p (real_dest, dest)
      || caller_info->node_dead)
    return false;
  if (!src->val)
    return true;

  if (caller_info->ipcp_orig_node)
    return (caller_info->known_contexts.length () > (unsigned) src->index)
      && values_equal_for_ipcp_p (src->val->value,
				  caller_info->known_contexts[src->index]);

  class ipcp_param_lattices *plats = ipa_get_parm_lattices (caller_info,
							     src->index);
  return plats->ctxlat.is_single_const ()
    && values_equal_for_ipcp_p (src->val->value,
				plats->ctxlat.values->value);
}