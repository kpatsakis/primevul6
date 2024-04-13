ipcp_discover_new_direct_edges (struct cgraph_node *node,
				vec<tree> known_csts,
				vec<ipa_polymorphic_call_context>
				known_contexts,
				struct ipa_agg_replacement_value *aggvals)
{
  struct cgraph_edge *ie, *next_ie;
  bool found = false;

  for (ie = node->indirect_calls; ie; ie = next_ie)
    {
      tree target;
      bool speculative;

      next_ie = ie->next_callee;
      target = ipa_get_indirect_edge_target_1 (ie, known_csts, known_contexts,
					       vNULL, aggvals, &speculative);
      if (target)
	{
	  bool agg_contents = ie->indirect_info->agg_contents;
	  bool polymorphic = ie->indirect_info->polymorphic;
	  int param_index = ie->indirect_info->param_index;
	  struct cgraph_edge *cs = ipa_make_edge_direct_to_target (ie, target,
								   speculative);
	  found = true;

	  if (cs && !agg_contents && !polymorphic)
	    {
	      class ipa_node_params *info = IPA_NODE_REF (node);
	      int c = ipa_get_controlled_uses (info, param_index);
	      if (c != IPA_UNDESCRIBED_USE)
		{
		  struct ipa_ref *to_del;

		  c--;
		  ipa_set_controlled_uses (info, param_index, c);
		  if (dump_file && (dump_flags & TDF_DETAILS))
		    fprintf (dump_file, "     controlled uses count of param "
			     "%i bumped down to %i\n", param_index, c);
		  if (c == 0
		      && (to_del = node->find_reference (cs->callee, NULL, 0)))
		    {
		      if (dump_file && (dump_flags & TDF_DETAILS))
			fprintf (dump_file, "       and even removing its "
				 "cloning-created reference\n");
		      to_del->remove_reference ();
		    }
		}
	    }
	}
    }
  /* Turning calls to direct calls will improve overall summary.  */
  if (found)
    ipa_update_overall_fn_summary (node);
}