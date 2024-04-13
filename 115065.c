agg_pass_through_permissible_p (class ipcp_param_lattices *src_plats,
				struct ipa_jump_func *jfunc)
{
  return src_plats->aggs
    && (!src_plats->aggs_by_ref
	|| ipa_get_jf_pass_through_agg_preserved (jfunc));
}