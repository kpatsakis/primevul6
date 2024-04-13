ipcp_store_bits_results (void)
{
  cgraph_node *node;

  FOR_EACH_FUNCTION_WITH_GIMPLE_BODY (node)
    {
      ipa_node_params *info = IPA_NODE_REF (node);
      bool dumped_sth = false;
      bool found_useful_result = false;

      if (!opt_for_fn (node->decl, flag_ipa_bit_cp) || !info)
	{
	  if (dump_file)
	    fprintf (dump_file, "Not considering %s for ipa bitwise propagation "
				"; -fipa-bit-cp: disabled.\n",
				node->name ());
	  continue;
	}

      if (info->ipcp_orig_node)
	info = IPA_NODE_REF (info->ipcp_orig_node);
      if (!info->lattices)
	/* Newly expanded artificial thunks do not have lattices.  */
	continue;

      unsigned count = ipa_get_param_count (info);
      for (unsigned i = 0; i < count; i++)
	{
	  ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
	  if (plats->bits_lattice.constant_p ())
	    {
	      found_useful_result = true;
	      break;
	    }
	}

      if (!found_useful_result)
	continue;

      ipcp_transformation_initialize ();
      ipcp_transformation *ts = ipcp_transformation_sum->get_create (node);
      vec_safe_reserve_exact (ts->bits, count);

      for (unsigned i = 0; i < count; i++)
	{
	  ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
	  ipa_bits *jfbits;

	  if (plats->bits_lattice.constant_p ())
	    jfbits
	      = ipa_get_ipa_bits_for_value (plats->bits_lattice.get_value (),
					    plats->bits_lattice.get_mask ());
	  else
	    jfbits = NULL;

	  ts->bits->quick_push (jfbits);
	  if (!dump_file || !jfbits)
	    continue;
	  if (!dumped_sth)
	    {
	      fprintf (dump_file, "Propagated bits info for function %s:\n",
		       node->dump_name ());
	      dumped_sth = true;
	    }
	  fprintf (dump_file, " param %i: value = ", i);
	  print_hex (jfbits->value, dump_file);
	  fprintf (dump_file, ", mask = ");
	  print_hex (jfbits->mask, dump_file);
	  fprintf (dump_file, "\n");
	}
    }
}