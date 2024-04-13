ipa_get_jf_pass_through_result (struct ipa_jump_func *jfunc, tree input,
				tree res_type)
{
  return ipa_get_jf_arith_result (ipa_get_jf_pass_through_operation (jfunc),
				  input,
				  ipa_get_jf_pass_through_operand (jfunc),
				  res_type);
}