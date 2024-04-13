ipa_get_indirect_edge_target (struct cgraph_edge *ie,
			      vec<tree> known_csts,
			      vec<ipa_polymorphic_call_context> known_contexts,
			      vec<ipa_agg_value_set> known_aggs,
			      bool *speculative)
{
  return ipa_get_indirect_edge_target_1 (ie, known_csts, known_contexts,
					 known_aggs, NULL, speculative);
}