want_remove_some_param_p (cgraph_node *node, vec<tree> known_csts)
{
  auto_vec<bool, 16> surviving;
  bool filled_vec = false;
  ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);

  for (i = 0; i < count; i++)
    {
      if (!known_csts[i] && ipa_is_param_used (info, i))
       continue;

      if (!filled_vec)
       {
         if (!node->clone.param_adjustments)
           return true;
         node->clone.param_adjustments->get_surviving_params (&surviving);
         filled_vec = true;
       }
      if (surviving.length() < (unsigned) i &&  surviving[i])
       return true;
    }
  return false;
}