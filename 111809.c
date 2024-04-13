bool InferenceContext::FullyDefined(ShapeHandle s) {
  if (!RankKnown(s)) return false;
  for (int i = 0; i < Rank(s); ++i) {
    if (!ValueKnown(Dim(s, i))) return false;
  }
  return true;
}