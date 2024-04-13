build_toporder_info (class ipa_topo_info *topo)
{
  topo->order = XCNEWVEC (struct cgraph_node *, symtab->cgraph_count);
  topo->stack = XCNEWVEC (struct cgraph_node *, symtab->cgraph_count);

  gcc_checking_assert (topo->stack_top == 0);
  topo->nnodes = ipa_reduced_postorder (topo->order, true,
					ignore_edge_p);
}