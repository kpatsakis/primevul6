gather_edges_for_value (ipcp_value<valtype> *val, cgraph_node *dest,
			int caller_count)
{
  ipcp_value_source<valtype> *src;
  vec<cgraph_edge *> ret;

  ret.create (caller_count);
  for (src = val->sources; src; src = src->next)
    {
      struct cgraph_edge *cs = src->cs;
      while (cs)
	{
	  if (cgraph_edge_brings_value_p (cs, src, dest, val))
	    ret.quick_push (cs);
	  cs = get_next_cgraph_edge_clone (cs);
	}
    }

  return ret;
}