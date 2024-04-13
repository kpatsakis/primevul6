Status InferenceContext::Run(
    const std::function<Status(shape_inference::InferenceContext* c)>& fn) {
  ForgetMerges();
  Status s = fn(this);
  if (!s.ok()) {
    ForgetMerges();
    return AttachContext(s);
  }
#ifndef NDEBUG
  for (int i = 0; i < num_outputs(); ++i) {
    DCHECK(output(i).IsSet()) << i << " for " << attrs_.SummarizeNode();
  }
#endif  // NDEBUG
  return s;
}