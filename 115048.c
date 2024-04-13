perform_estimation_of_a_value (cgraph_node *node, vec<tree> known_csts,
			       vec<ipa_polymorphic_call_context> known_contexts,
			       vec<ipa_agg_value_set> known_aggs,
			       int removable_params_cost,
			       int est_move_cost, ipcp_value_base *val)
{
  int size, time_benefit;
  sreal time, base_time;
  ipa_hints hints;

  estimate_ipcp_clone_size_and_time (node, known_csts, known_contexts,
				     known_aggs, &size, &time,
				     &base_time, &hints);
  base_time -= time;
  if (base_time > 65535)
    base_time = 65535;

  /* Extern inline functions have no cloning local time benefits because they
     will be inlined anyway.  The only reason to clone them is if it enables
     optimization in any of the functions they call.  */
  if (DECL_EXTERNAL (node->decl) && DECL_DECLARED_INLINE_P (node->decl))
    time_benefit = 0;
  else
    time_benefit = base_time.to_int ()
      + devirtualization_time_bonus (node, known_csts, known_contexts,
				     known_aggs)
      + hint_time_bonus (hints)
      + removable_params_cost + est_move_cost;

  gcc_checking_assert (size >=0);
  /* The inliner-heuristics based estimates may think that in certain
     contexts some functions do not have any size at all but we want
     all specializations to have at least a tiny cost, not least not to
     divide by zero.  */
  if (size == 0)
    size = 1;

  val->local_time_benefit = time_benefit;
  val->local_size_cost = size;
}