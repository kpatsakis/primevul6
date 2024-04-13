self_recursive_pass_through_p (cgraph_edge *cs, ipa_jump_func *jfunc, int i)
{
  enum availability availability;
  if (cs->caller == cs->callee->function_symbol (&availability)
      && availability > AVAIL_INTERPOSABLE
      && jfunc->type == IPA_JF_PASS_THROUGH
      && ipa_get_jf_pass_through_operation (jfunc) == NOP_EXPR
      && ipa_get_jf_pass_through_formal_id (jfunc) == i)
    return true;
  return false;
}