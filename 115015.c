free_toporder_info (class ipa_topo_info *topo)
{
  ipa_free_postorder_info ();
  free (topo->order);
  free (topo->stack);
}