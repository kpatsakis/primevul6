ipcp_driver (void)
{
  class ipa_topo_info topo;

  if (edge_clone_summaries == NULL)
    edge_clone_summaries = new edge_clone_summary_t (symtab);

  ipa_check_create_node_params ();
  ipa_check_create_edge_args ();
  clone_num_suffixes = new hash_map<const char *, unsigned>;

  if (dump_file)
    {
      fprintf (dump_file, "\nIPA structures before propagation:\n");
      if (dump_flags & TDF_DETAILS)
	ipa_print_all_params (dump_file);
      ipa_print_all_jump_functions (dump_file);
    }

  /* Topological sort.  */
  build_toporder_info (&topo);
  /* Do the interprocedural propagation.  */
  ipcp_propagate_stage (&topo);
  /* Decide what constant propagation and cloning should be performed.  */
  ipcp_decision_stage (&topo);
  /* Store results of bits propagation.  */
  ipcp_store_bits_results ();
  /* Store results of value range propagation.  */
  ipcp_store_vr_results ();

  /* Free all IPCP structures.  */
  delete clone_num_suffixes;
  free_toporder_info (&topo);
  delete edge_clone_summaries;
  edge_clone_summaries = NULL;
  ipa_free_all_structures_after_ipa_cp ();
  if (dump_file)
    fprintf (dump_file, "\nIPA constant propagation end\n");
  return 0;
}