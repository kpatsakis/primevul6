update_specialized_profile (struct cgraph_node *new_node,
			    struct cgraph_node *orig_node,
			    profile_count redirected_sum)
{
  struct cgraph_edge *cs;
  profile_count new_node_count, orig_node_count = orig_node->count;

  if (dump_file)
    {
      fprintf (dump_file, "    the sum of counts of redirected  edges is ");
      redirected_sum.dump (dump_file);
      fprintf (dump_file, "\n");
    }
  if (!(orig_node_count > profile_count::zero ()))
    return;

  gcc_assert (orig_node_count >= redirected_sum);

  new_node_count = new_node->count;
  new_node->count += redirected_sum;
  orig_node->count -= redirected_sum;

  for (cs = new_node->callees; cs; cs = cs->next_callee)
    cs->count += cs->count.apply_scale (redirected_sum, new_node_count);

  for (cs = orig_node->callees; cs; cs = cs->next_callee)
    {
      profile_count dec = cs->count.apply_scale (redirected_sum,
						 orig_node_count);
      cs->count -= dec;
    }

  if (dump_file)
    dump_profile_updates (orig_node, new_node);
}