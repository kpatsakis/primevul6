create_specialized_node (struct cgraph_node *node,
			 vec<tree> known_csts,
			 vec<ipa_polymorphic_call_context> known_contexts,
			 struct ipa_agg_replacement_value *aggvals,
			 vec<cgraph_edge *> callers)
{
  class ipa_node_params *new_info, *info = IPA_NODE_REF (node);
  vec<ipa_replace_map *, va_gc> *replace_trees = NULL;
  vec<ipa_adjusted_param, va_gc> *new_params = NULL;
  struct ipa_agg_replacement_value *av;
  struct cgraph_node *new_node;
  int i, count = ipa_get_param_count (info);
  ipa_param_adjustments *old_adjustments = node->clone.param_adjustments;
  ipa_param_adjustments *new_adjustments;
  gcc_assert (!info->ipcp_orig_node);
  gcc_assert (node->can_change_signature
	      || !old_adjustments);

  if (old_adjustments)
    {
      /* At the moment all IPA optimizations should use the number of
	 parameters of the prevailing decl as the m_always_copy_start.
	 Handling any other value would complicate the code below, so for the
	 time bing let's only assert it is so.  */
      gcc_assert (old_adjustments->m_always_copy_start == count
		  || old_adjustments->m_always_copy_start < 0);
      int old_adj_count = vec_safe_length (old_adjustments->m_adj_params);
      for (i = 0; i < old_adj_count; i++)
	{
	  ipa_adjusted_param *old_adj = &(*old_adjustments->m_adj_params)[i];
	  if (!node->can_change_signature
	      || old_adj->op != IPA_PARAM_OP_COPY
	      || (!known_csts[old_adj->base_index]
		  && ipa_is_param_used (info, old_adj->base_index)))
	    {
	      ipa_adjusted_param new_adj = *old_adj;

	      new_adj.prev_clone_adjustment = true;
	      new_adj.prev_clone_index = i;
	      vec_safe_push (new_params, new_adj);
	    }
	}
      bool skip_return = old_adjustments->m_skip_return;
      new_adjustments = (new (ggc_alloc <ipa_param_adjustments> ())
			 ipa_param_adjustments (new_params, count,
						skip_return));
    }
  else if (node->can_change_signature
	   && want_remove_some_param_p (node, known_csts))
    {
      ipa_adjusted_param adj;
      memset (&adj, 0, sizeof (adj));
      adj.op = IPA_PARAM_OP_COPY;
      for (i = 0; i < count; i++)
	if (!known_csts[i] && ipa_is_param_used (info, i))
	  {
	    adj.base_index = i;
	    adj.prev_clone_index = i;
	    vec_safe_push (new_params, adj);
	  }
      new_adjustments = (new (ggc_alloc <ipa_param_adjustments> ())
			 ipa_param_adjustments (new_params, count, false));
    }
  else
    new_adjustments = NULL;

  replace_trees = vec_safe_copy (node->clone.tree_map);
  for (i = 0; i < count; i++)
    {
      tree t = known_csts[i];
      if (t)
	{
	  struct ipa_replace_map *replace_map;

	  gcc_checking_assert (TREE_CODE (t) != TREE_BINFO);
	  replace_map = get_replacement_map (info, t, i);
	  if (replace_map)
	    vec_safe_push (replace_trees, replace_map);
	}
    }
  auto_vec<cgraph_edge *, 2> self_recursive_calls;
  for (i = callers.length () - 1; i >= 0; i--)
    {
      cgraph_edge *cs = callers[i];
      if (cs->caller == node)
	{
	  self_recursive_calls.safe_push (cs);
	  callers.unordered_remove (i);
	}
    }

  unsigned &suffix_counter = clone_num_suffixes->get_or_insert (
			       IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (
				 node->decl)));
  new_node = node->create_virtual_clone (callers, replace_trees,
					 new_adjustments, "constprop",
					 suffix_counter);
  suffix_counter++;

  bool have_self_recursive_calls = !self_recursive_calls.is_empty ();
  for (unsigned j = 0; j < self_recursive_calls.length (); j++)
    {
      cgraph_edge *cs = get_next_cgraph_edge_clone (self_recursive_calls[j]);
      /* Cloned edges can disappear during cloning as speculation can be
	 resolved, check that we have one and that it comes from the last
	 cloning.  */
      if (cs && cs->caller == new_node)
	cs->redirect_callee_duplicating_thunks (new_node);
      /* Any future code that would make more than one clone of an outgoing
	 edge would confuse this mechanism, so let's check that does not
	 happen.  */
      gcc_checking_assert (!cs
			   || !get_next_cgraph_edge_clone (cs)
			   || get_next_cgraph_edge_clone (cs)->caller != new_node);
    }
  if (have_self_recursive_calls)
    new_node->expand_all_artificial_thunks ();

  ipa_set_node_agg_value_chain (new_node, aggvals);
  for (av = aggvals; av; av = av->next)
    new_node->maybe_create_reference (av->value, NULL);

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "     the new node is %s.\n", new_node->dump_name ());
      if (known_contexts.exists ())
	{
	  for (i = 0; i < count; i++)
	    if (!known_contexts[i].useless_p ())
	      {
		fprintf (dump_file, "     known ctx %i is ", i);
		known_contexts[i].dump (dump_file);
	      }
	}
      if (aggvals)
	ipa_dump_agg_replacement_values (dump_file, aggvals);
    }
  ipa_check_create_node_params ();
  update_profiling_info (node, new_node);
  new_info = IPA_NODE_REF (new_node);
  new_info->ipcp_orig_node = node;
  new_node->ipcp_clone = true;
  new_info->known_csts = known_csts;
  new_info->known_contexts = known_contexts;

  ipcp_discover_new_direct_edges (new_node, known_csts, known_contexts, aggvals);

  callers.release ();
  return new_node;
}