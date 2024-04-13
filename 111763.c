string InferenceContext::DebugString() const {
  return strings::StrCat("InferenceContext for node: ", attrs_.SummarizeNode());
}