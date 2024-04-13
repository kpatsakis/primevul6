get_info_about_necessary_edges (ipcp_value<valtype> *val, cgraph_node *dest,
				int *freq_sum,
				profile_count *count_sum, int *caller_count)
{
  ipcp_value_source<valtype> *src;
  int freq = 0, count = 0;
  profile_count cnt = profile_count::zero ();
  bool hot = false;
  bool non_self_recursive = false;

  for (src = val->sources; src; src = src->next)
    {
      struct cgraph_edge *cs = src->cs;
      while (cs)
	{
	  if (cgraph_edge_brings_value_p (cs, src, dest, val))
	    {
	      count++;
	      freq += cs->frequency ();
	      if (cs->count.ipa ().initialized_p ())
	        cnt += cs->count.ipa ();
	      hot |= cs->maybe_hot_p ();
	      if (cs->caller != dest)
		non_self_recursive = true;
	      else if (src->val)
		gcc_assert (values_equal_for_ipcp_p (src->val->value,
						     val->value));
	    }
	  cs = get_next_cgraph_edge_clone (cs);
	}
    }

  /* If the only edges bringing a value are self-recursive ones, do not bother
     evaluating it.  */
  if (!non_self_recursive)
    return false;

  *freq_sum = freq;
  *count_sum = cnt;
  *caller_count = count;

  if (!hot && IPA_NODE_REF (dest)->node_within_scc)
    {
      struct cgraph_edge *cs;

      /* Cold non-SCC source edge could trigger hot recursive execution of
	 function. Consider the case as hot and rely on following cost model
	 computation to further select right one.  */
      for (cs = dest->callers; cs; cs = cs->next_caller)
	if (cs->caller == dest && cs->maybe_hot_p ())
	  return true;
    }

  return hot;
}