allocate_and_init_ipcp_value (ipa_polymorphic_call_context source)
{
  ipcp_value<ipa_polymorphic_call_context> *val;

  // TODO
  val = new (ipcp_poly_ctx_values_pool.allocate ())
    ipcp_value<ipa_polymorphic_call_context>();
  val->value = source;
  return val;
}