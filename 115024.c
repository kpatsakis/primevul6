ipa_vr_operation_and_type_effects (value_range *dst_vr,
				   value_range *src_vr,
				   enum tree_code operation,
				   tree dst_type, tree src_type)
{
  range_fold_unary_expr (dst_vr, operation, dst_type, src_vr, src_type);
  if (dst_vr->varying_p () || dst_vr->undefined_p ())
    return false;
  return true;
}