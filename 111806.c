string InferenceContext::DebugString(
    gtl::ArraySlice<ShapeAndType> shape_and_types) {
  std::vector<string> pieces;
  for (const ShapeAndType& s : shape_and_types) {
    pieces.push_back(DebugString(s));
  }
  return strings::StrCat("[", absl::StrJoin(pieces, ","), "]");
}