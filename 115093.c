propagate_scalar_across_jump_function (struct cgraph_edge *cs,
				       struct ipa_jump_func *jfunc,
				       ipcp_lattice<tree> *dest_lat,
				       tree param_type)
{
  if (dest_lat->bottom)
    return false;

  if (jfunc->type == IPA_JF_CONST)
    {
      tree val = ipa_get_jf_constant (jfunc);
      return dest_lat->add_value (val, cs, NULL, 0);
    }
  else if (jfunc->type == IPA_JF_PASS_THROUGH
	   || jfunc->type == IPA_JF_ANCESTOR)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      ipcp_lattice<tree> *src_lat;
      int src_idx;
      bool ret;

      if (jfunc->type == IPA_JF_PASS_THROUGH)
	src_idx = ipa_get_jf_pass_through_formal_id (jfunc);
      else
	src_idx = ipa_get_jf_ancestor_formal_id (jfunc);

      src_lat = ipa_get_scalar_lat (caller_info, src_idx);
      if (src_lat->bottom)
	return dest_lat->set_contains_variable ();

      /* If we would need to clone the caller and cannot, do not propagate.  */
      if (!ipcp_versionable_function_p (cs->caller)
	  && (src_lat->contains_variable
	      || (src_lat->values_count > 1)))
	return dest_lat->set_contains_variable ();

      if (jfunc->type == IPA_JF_PASS_THROUGH)
	ret = propagate_vals_across_pass_through (cs, jfunc, src_lat,
						  dest_lat, src_idx, param_type);
      else
	ret = propagate_vals_across_ancestor (cs, jfunc, src_lat, dest_lat,
					      src_idx);

      if (src_lat->contains_variable)
	ret |= dest_lat->set_contains_variable ();

      return ret;
    }

  /* TODO: We currently do not handle member method pointers in IPA-CP (we only
     use it for indirect inlining), we should propagate them too.  */
  return dest_lat->set_contains_variable ();
}