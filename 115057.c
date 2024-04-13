ipa_get_scalar_lat (class ipa_node_params *info, int i)
{
  class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
  return &plats->itself;
}