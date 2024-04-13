const TensorSliceSet* TensorSliceReader::FindTensorSlice(
    const string& name, const TensorSlice& slice,
    std::vector<std::pair<TensorSlice, string>>* details) const {
  const TensorSliceSet* tss = gtl::FindPtrOrNull(tensors_, name);
  if (tss && !tss->QueryMeta(slice, details)) {
    return nullptr;
  }
  return tss;
}