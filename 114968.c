incorporate_penalties (ipa_node_params *info, int64_t evaluation)
{
  if (info->node_within_scc && !info->node_is_self_scc)
    evaluation = (evaluation
		  * (100 - param_ipa_cp_recursion_penalty)) / 100;

  if (info->node_calling_single_call)
    evaluation = (evaluation
		  * (100 - param_ipa_cp_single_call_penalty))
      / 100;

  return evaluation;
}