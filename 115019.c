propagate_vals_across_ancestor (struct cgraph_edge *cs,
				struct ipa_jump_func *jfunc,
				ipcp_lattice<tree> *src_lat,
				ipcp_lattice<tree> *dest_lat, int src_idx)
{
  ipcp_value<tree> *src_val;
  bool ret = false;

  if (ipa_edge_within_scc (cs))
    return dest_lat->set_contains_variable ();

  for (src_val = src_lat->values; src_val; src_val = src_val->next)
    {
      tree t = ipa_get_jf_ancestor_result (jfunc, src_val->value);

      if (t)
	ret |= dest_lat->add_value (t, cs, src_val, src_idx);
      else
	ret |= dest_lat->set_contains_variable ();
    }

  return ret;
}