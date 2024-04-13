ipcp_propagate_stage (class ipa_topo_info *topo)
{
  struct cgraph_node *node;

  if (dump_file)
    fprintf (dump_file, "\n Propagating constants:\n\n");

  max_count = profile_count::uninitialized ();

  FOR_EACH_DEFINED_FUNCTION (node)
  {
    if (node->has_gimple_body_p ()
	&& opt_for_fn (node->decl, flag_ipa_cp)
	&& opt_for_fn (node->decl, optimize))
      {
        class ipa_node_params *info = IPA_NODE_REF (node);
        determine_versionability (node, info);
	info->lattices = XCNEWVEC (class ipcp_param_lattices,
				   ipa_get_param_count (info));
	initialize_node_lattices (node);
      }
    ipa_size_summary *s = ipa_size_summaries->get (node);
    if (node->definition && !node->alias && s != NULL)
      overall_size += s->self_size;
    max_count = max_count.max (node->count.ipa ());
  }

  max_new_size = overall_size;
  if (max_new_size < param_large_unit_insns)
    max_new_size = param_large_unit_insns;
  max_new_size += max_new_size * param_ipcp_unit_growth / 100 + 1;

  if (dump_file)
    fprintf (dump_file, "\noverall_size: %li, max_new_size: %li\n",
	     overall_size, max_new_size);

  propagate_constants_topo (topo);
  if (flag_checking)
    ipcp_verify_propagated_values ();
  topo->constants.propagate_effects ();
  topo->contexts.propagate_effects ();

  if (dump_file)
    {
      fprintf (dump_file, "\nIPA lattices after all propagation:\n");
      print_all_lattices (dump_file, (dump_flags & TDF_DETAILS), true);
    }
}