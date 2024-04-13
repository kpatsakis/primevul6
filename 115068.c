ipa_get_parm_lattices (class ipa_node_params *info, int i)
{
  gcc_assert (i >= 0 && i < ipa_get_param_count (info));
  gcc_checking_assert (!info->ipcp_orig_node);
  gcc_checking_assert (info->lattices);
  return &(info->lattices[i]);
}