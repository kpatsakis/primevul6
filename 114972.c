ignore_edge_p (cgraph_edge *e)
{
  enum availability avail;
  cgraph_node *ultimate_target
    = e->callee->function_or_virtual_thunk_symbol (&avail, e->caller);

  return (avail <= AVAIL_INTERPOSABLE
	  || !opt_for_fn (ultimate_target->decl, optimize)
	  || !opt_for_fn (ultimate_target->decl, flag_ipa_cp));
}