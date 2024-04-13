Attribute ConvertSingleElementAttrToFloatAttr(Attribute attr) {
  const auto dense_fp_attr = attr.dyn_cast_or_null<DenseFPElementsAttr>();
  if (dense_fp_attr) {
    // Already float => return
    return dense_fp_attr;
  }

  OpBuilder builder(attr.getContext());

  const auto dense_int_attr = attr.dyn_cast<DenseIntElementsAttr>();
  const auto int_values = dense_int_attr.getIntValues();
  float float_val = 0.0f;
  if (!int_values.empty()) {
    const APInt apint_val = *int_values.begin();
    if (dense_int_attr.getType().getElementType().isSignedInteger()) {
      // Get the sign-extended value (=>int64) if the type is signed.
      float_val = apint_val.getSExtValue();
    } else {
      // Get the zero-extended value (=>uint64) if unsigned or signless.
      float_val = apint_val.getZExtValue();
    }
  }
  return DenseFPElementsAttr::get(
      RankedTensorType::get({}, builder.getF32Type()),
      {llvm::APFloat(float_val)});
}