string InferenceContext::DebugString(ShapeHandle s) {
  if (RankKnown(s)) {
    std::vector<string> vals;
    for (auto d : s->dims_) vals.push_back(DebugString(d));
    return strings::StrCat("[", absl::StrJoin(vals, ","), "]");
  } else {
    return "?";
  }
}