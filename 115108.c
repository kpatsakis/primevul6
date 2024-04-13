good_cloning_opportunity_p (struct cgraph_node *node, int time_benefit,
			    int freq_sum, profile_count count_sum, int size_cost)
{
  if (time_benefit == 0
      || !opt_for_fn (node->decl, flag_ipa_cp_clone)
      || node->optimize_for_size_p ())
    return false;

  gcc_assert (size_cost > 0);

  class ipa_node_params *info = IPA_NODE_REF (node);
  if (max_count > profile_count::zero ())
    {
      int factor = RDIV (count_sum.probability_in
				 (max_count).to_reg_br_prob_base ()
		         * 1000, REG_BR_PROB_BASE);
      int64_t evaluation = (((int64_t) time_benefit * factor)
				    / size_cost);
      evaluation = incorporate_penalties (info, evaluation);

      if (dump_file && (dump_flags & TDF_DETAILS))
	{
	  fprintf (dump_file, "     good_cloning_opportunity_p (time: %i, "
		   "size: %i, count_sum: ", time_benefit, size_cost);
	  count_sum.dump (dump_file);
	  fprintf (dump_file, "%s%s) -> evaluation: " "%" PRId64
		 ", threshold: %i\n",
		 info->node_within_scc
		   ? (info->node_is_self_scc ? ", self_scc" : ", scc") : "",
		 info->node_calling_single_call ? ", single_call" : "",
		 evaluation, param_ipa_cp_eval_threshold);
	}

      return evaluation >= param_ipa_cp_eval_threshold;
    }
  else
    {
      int64_t evaluation = (((int64_t) time_benefit * freq_sum)
				    / size_cost);
      evaluation = incorporate_penalties (info, evaluation);

      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "     good_cloning_opportunity_p (time: %i, "
		 "size: %i, freq_sum: %i%s%s) -> evaluation: "
		 "%" PRId64 ", threshold: %i\n",
		 time_benefit, size_cost, freq_sum,
		 info->node_within_scc
		   ? (info->node_is_self_scc ? ", self_scc" : ", scc") : "",
		 info->node_calling_single_call ? ", single_call" : "",
		 evaluation, param_ipa_cp_eval_threshold);

      return evaluation >= param_ipa_cp_eval_threshold;
    }
}