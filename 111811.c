string InferenceContext::DebugString(const ShapeAndType& shape_and_type) {
  return strings::StrCat(DebugString(shape_and_type.shape), ":",
                         DataTypeString(shape_and_type.dtype));
}