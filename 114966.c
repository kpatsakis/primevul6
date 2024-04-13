get_next_cgraph_edge_clone (struct cgraph_edge *cs)
{
  edge_clone_summary *s = edge_clone_summaries->get (cs);
  return s != NULL ? s->next_clone : NULL;
}