propagate_vals_across_arith_jfunc (cgraph_edge *cs,
				   enum tree_code opcode,
				   tree opnd1_type,
				   tree opnd2,
				   ipcp_lattice<tree> *src_lat,
				   ipcp_lattice<tree> *dest_lat,
				   HOST_WIDE_INT src_offset,
				   int src_idx,
				   tree res_type)
{
  ipcp_value<tree> *src_val;
  bool ret = false;

  /* Due to circular dependencies, propagating within an SCC through arithmetic
     transformation would create infinite number of values.  But for
     self-feeding recursive function, we could allow propagation in a limited
     count, and this can enable a simple kind of recursive function versioning.
     For other scenario, we would just make lattices bottom.  */
  if (opcode != NOP_EXPR && ipa_edge_within_scc (cs))
    {
      int i;

      if (src_lat != dest_lat || param_ipa_cp_max_recursive_depth < 1)
	return dest_lat->set_contains_variable ();

      /* No benefit if recursive execution is in low probability.  */
      if (cs->sreal_frequency () * 100
	  <= ((sreal) 1) * param_ipa_cp_min_recursive_probability)
	return dest_lat->set_contains_variable ();

      auto_vec<ipcp_value<tree> *, 8> val_seeds;

      for (src_val = src_lat->values; src_val; src_val = src_val->next)
	{
	  /* Now we do not use self-recursively generated value as propagation
	     source, this is absolutely conservative, but could avoid explosion
	     of lattice's value space, especially when one recursive function
	     calls another recursive.  */
	  if (self_recursively_generated_p (src_val))
	    {
	      ipcp_value_source<tree> *s;

	      /* If the lattice has already been propagated for the call site,
		 no need to do that again.  */
	      for (s = src_val->sources; s; s = s->next)
		if (s->cs == cs)
		  return dest_lat->set_contains_variable ();
	    }
	  else
	    val_seeds.safe_push (src_val);
	}

      /* Recursively generate lattice values with a limited count.  */
      FOR_EACH_VEC_ELT (val_seeds, i, src_val)
	{
	  for (int j = 1; j < param_ipa_cp_max_recursive_depth; j++)
	    {
	      tree cstval = get_val_across_arith_op (opcode, opnd1_type, opnd2,
						     src_val, res_type);
	      if (!cstval)
		break;

	      ret |= dest_lat->add_value (cstval, cs, src_val, src_idx,
					  src_offset, &src_val, true);
	      gcc_checking_assert (src_val);
	    }
	}
      ret |= dest_lat->set_contains_variable ();
    }
  else
    for (src_val = src_lat->values; src_val; src_val = src_val->next)
      {
	/* Now we do not use self-recursively generated value as propagation
	   source, otherwise it is easy to make value space of normal lattice
	   overflow.  */
	if (self_recursively_generated_p (src_val))
	  {
	    ret |= dest_lat->set_contains_variable ();
	    continue;
	  }

	tree cstval = get_val_across_arith_op (opcode, opnd1_type, opnd2,
					       src_val, res_type);
	if (cstval)
	  ret |= dest_lat->add_value (cstval, cs, src_val, src_idx,
				      src_offset);
	else
	  ret |= dest_lat->set_contains_variable ();
      }

  return ret;
}