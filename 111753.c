string InferenceContext::DebugString(DimensionHandle d) {
  return ValueKnown(d) ? strings::StrCat(Value(d)) : "?";
}