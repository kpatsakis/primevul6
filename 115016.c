propagate_vals_across_pass_through (cgraph_edge *cs, ipa_jump_func *jfunc,
				    ipcp_lattice<tree> *src_lat,
				    ipcp_lattice<tree> *dest_lat, int src_idx,
				    tree parm_type)
{
  return propagate_vals_across_arith_jfunc (cs,
				ipa_get_jf_pass_through_operation (jfunc),
				NULL_TREE,
				ipa_get_jf_pass_through_operand (jfunc),
				src_lat, dest_lat, -1, src_idx, parm_type);
}