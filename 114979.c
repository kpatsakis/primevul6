ipcp_verify_propagated_values (void)
{
  struct cgraph_node *node;

  FOR_EACH_FUNCTION_WITH_GIMPLE_BODY (node)
    {
      class ipa_node_params *info = IPA_NODE_REF (node);
      if (!opt_for_fn (node->decl, flag_ipa_cp)
	  || !opt_for_fn (node->decl, optimize))
	continue;
      int i, count = ipa_get_param_count (info);

      for (i = 0; i < count; i++)
	{
	  ipcp_lattice<tree> *lat = ipa_get_scalar_lat (info, i);

	  if (!lat->bottom
	      && !lat->contains_variable
	      && lat->values_count == 0)
	    {
	      if (dump_file)
		{
		  symtab->dump (dump_file);
		  fprintf (dump_file, "\nIPA lattices after constant "
			   "propagation, before gcc_unreachable:\n");
		  print_all_lattices (dump_file, true, false);
		}

	      gcc_unreachable ();
	    }
	}
    }
}