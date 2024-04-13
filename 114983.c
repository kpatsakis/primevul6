decide_about_value (struct cgraph_node *node, int index, HOST_WIDE_INT offset,
		    ipcp_value<valtype> *val, vec<tree> known_csts,
		    vec<ipa_polymorphic_call_context> known_contexts)
{
  struct ipa_agg_replacement_value *aggvals;
  int freq_sum, caller_count;
  profile_count count_sum;
  vec<cgraph_edge *> callers;

  if (val->spec_node)
    {
      perhaps_add_new_callers (node, val);
      return false;
    }
  else if (val->local_size_cost + overall_size > max_new_size)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "   Ignoring candidate value because "
		 "max_new_size would be reached with %li.\n",
		 val->local_size_cost + overall_size);
      return false;
    }
  else if (!get_info_about_necessary_edges (val, node, &freq_sum, &count_sum,
					    &caller_count))
    return false;

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, " - considering value ");
      print_ipcp_constant_value (dump_file, val->value);
      fprintf (dump_file, " for ");
      ipa_dump_param (dump_file, IPA_NODE_REF (node), index);
      if (offset != -1)
	fprintf (dump_file, ", offset: " HOST_WIDE_INT_PRINT_DEC, offset);
      fprintf (dump_file, " (caller_count: %i)\n", caller_count);
    }

  if (!good_cloning_opportunity_p (node, val->local_time_benefit,
				   freq_sum, count_sum,
				   val->local_size_cost)
      && !good_cloning_opportunity_p (node,
				      val->local_time_benefit
				      + val->prop_time_benefit,
				      freq_sum, count_sum,
				      val->local_size_cost
				      + val->prop_size_cost))
    return false;

  if (dump_file)
    fprintf (dump_file, "  Creating a specialized node of %s.\n",
	     node->dump_name ());

  callers = gather_edges_for_value (val, node, caller_count);
  if (offset == -1)
    modify_known_vectors_with_val (&known_csts, &known_contexts, val, index);
  else
    {
      known_csts = known_csts.copy ();
      known_contexts = copy_useful_known_contexts (known_contexts);
    }
  find_more_scalar_values_for_callers_subset (node, known_csts, callers);
  find_more_contexts_for_caller_subset (node, &known_contexts, callers);
  aggvals = find_aggregate_values_for_callers_subset (node, callers);
  gcc_checking_assert (ipcp_val_agg_replacement_ok_p (aggvals, index,
						      offset, val->value));
  val->spec_node = create_specialized_node (node, known_csts, known_contexts,
					    aggvals, callers);
  overall_size += val->local_size_cost;

  /* TODO: If for some lattice there is only one other known value
     left, make a special node for it too. */

  return true;
}