ipcp_val_agg_replacement_ok_p (ipa_agg_replacement_value *,
			       int , HOST_WIDE_INT offset,
			       ipa_polymorphic_call_context)
{
  return offset == -1;
}