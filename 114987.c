allocate_and_init_ipcp_value (tree source)
{
  ipcp_value<tree> *val;

  val = new (ipcp_cst_values_pool.allocate ()) ipcp_value<tree>();
  val->value = source;
  return val;
}