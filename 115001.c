set_chain_of_aglats_contains_variable (struct ipcp_agg_lattice *aglat)
{
  bool ret = false;
  while (aglat)
    {
      ret |= aglat->set_contains_variable ();
      aglat = aglat->next;
    }
  return ret;
}