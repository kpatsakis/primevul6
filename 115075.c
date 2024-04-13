call_passes_through_thunk_p (cgraph_edge *cs)
{
  cgraph_node *alias_or_thunk = cs->callee;
  while (alias_or_thunk->alias)
    alias_or_thunk = alias_or_thunk->get_alias_target ();
  return alias_or_thunk->thunk.thunk_p;
}