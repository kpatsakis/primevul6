ipa_value_from_jfunc (class ipa_node_params *info, struct ipa_jump_func *jfunc,
		      tree parm_type)
{
  if (jfunc->type == IPA_JF_CONST)
    return ipa_get_jf_constant (jfunc);
  else if (jfunc->type == IPA_JF_PASS_THROUGH
	   || jfunc->type == IPA_JF_ANCESTOR)
    {
      tree input;
      int idx;

      if (jfunc->type == IPA_JF_PASS_THROUGH)
	idx = ipa_get_jf_pass_through_formal_id (jfunc);
      else
	idx = ipa_get_jf_ancestor_formal_id (jfunc);

      if (info->ipcp_orig_node)
	input = info->known_csts[idx];
      else
	{
	  ipcp_lattice<tree> *lat;

	  if (!info->lattices
	      || idx >= ipa_get_param_count (info))
	    return NULL_TREE;
	  lat = ipa_get_scalar_lat (info, idx);
	  if (!lat->is_single_const ())
	    return NULL_TREE;
	  input = lat->values->value;
	}

      if (!input)
	return NULL_TREE;

      if (jfunc->type == IPA_JF_PASS_THROUGH)
	return ipa_get_jf_pass_through_result (jfunc, input, parm_type);
      else
	return ipa_get_jf_ancestor_result (jfunc, input);
    }
  else
    return NULL_TREE;
}