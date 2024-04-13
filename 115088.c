self_recursive_agg_pass_through_p (cgraph_edge *cs, ipa_agg_jf_item *jfunc,
				   int i)
{
  enum availability availability;
  if (cs->caller == cs->callee->function_symbol (&availability)
      && availability > AVAIL_INTERPOSABLE
      && jfunc->jftype == IPA_JF_LOAD_AGG
      && jfunc->offset == jfunc->value.load_agg.offset
      && jfunc->value.pass_through.operation == NOP_EXPR
      && jfunc->value.pass_through.formal_id == i)
    return true;
  return false;
}