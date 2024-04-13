devirtualization_time_bonus (struct cgraph_node *node,
			     vec<tree> known_csts,
			     vec<ipa_polymorphic_call_context> known_contexts,
			     vec<ipa_agg_value_set> known_aggs)
{
  struct cgraph_edge *ie;
  int res = 0;

  for (ie = node->indirect_calls; ie; ie = ie->next_callee)
    {
      struct cgraph_node *callee;
      class ipa_fn_summary *isummary;
      enum availability avail;
      tree target;
      bool speculative;

      target = ipa_get_indirect_edge_target (ie, known_csts, known_contexts,
					     known_aggs, &speculative);
      if (!target)
	continue;

      /* Only bare minimum benefit for clearly un-inlineable targets.  */
      res += 1;
      callee = cgraph_node::get (target);
      if (!callee || !callee->definition)
	continue;
      callee = callee->function_symbol (&avail);
      if (avail < AVAIL_AVAILABLE)
	continue;
      isummary = ipa_fn_summaries->get (callee);
      if (!isummary->inlinable)
	continue;

      int size = ipa_size_summaries->get (callee)->size;
      /* FIXME: The values below need re-considering and perhaps also
	 integrating into the cost metrics, at lest in some very basic way.  */
      int max_inline_insns_auto
	= opt_for_fn (callee->decl, param_max_inline_insns_auto);
      if (size <= max_inline_insns_auto / 4)
	res += 31 / ((int)speculative + 1);
      else if (size <= max_inline_insns_auto / 2)
	res += 15 / ((int)speculative + 1);
      else if (size <= max_inline_insns_auto
	       || DECL_DECLARED_INLINE_P (callee->decl))
	res += 7 / ((int)speculative + 1);
    }

  return res;
}