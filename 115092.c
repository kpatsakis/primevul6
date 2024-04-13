dump_profile_updates (struct cgraph_node *orig_node,
		      struct cgraph_node *new_node)
{
  struct cgraph_edge *cs;

  fprintf (dump_file, "    setting count of the specialized node to ");
  new_node->count.dump (dump_file);
  fprintf (dump_file, "\n");
  for (cs = new_node->callees; cs; cs = cs->next_callee)
    {
      fprintf (dump_file, "      edge to %s has count ",
	       cs->callee->name ());
      cs->count.dump (dump_file);
      fprintf (dump_file, "\n");
    }

  fprintf (dump_file, "    setting count of the original node to ");
  orig_node->count.dump (dump_file);
  fprintf (dump_file, "\n");
  for (cs = orig_node->callees; cs; cs = cs->next_callee)
    {
      fprintf (dump_file, "      edge to %s is left with ",
	       cs->callee->name ());
      cs->count.dump (dump_file);
      fprintf (dump_file, "\n");
    }
}