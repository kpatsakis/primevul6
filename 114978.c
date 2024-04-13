count_callers (cgraph_node *node, void *data)
{
  int *caller_count = (int *) data;

  for (cgraph_edge *cs = node->callers; cs; cs = cs->next_caller)
    /* Local thunks can be handled transparently, but if the thunk cannot
       be optimized out, count it as a real use.  */
    if (!cs->caller->thunk.thunk_p || !cs->caller->local)
      ++*caller_count;
  return false;
}