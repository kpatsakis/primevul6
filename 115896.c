static bool FloatValueEquals(const Attribute &attr, double value) {
  auto fp_attr = attr.dyn_cast_or_null<DenseFPElementsAttr>();
  if (!fp_attr) return false;

  if (fp_attr.isSplat()) {
    return fp_attr.getSplatValue<APFloat>().isExactlyValue(value);
  }
  return llvm::all_of(fp_attr.getFloatValues(), [value](const APFloat &f) {
    return f.isExactlyValue(value);
  });
}