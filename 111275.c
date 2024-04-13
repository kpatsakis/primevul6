TensorSlice::TensorSlice(
    std::initializer_list<std::pair<int64_t, int64_t>> extents) {
  starts_.reserve(extents.size());
  lengths_.reserve(extents.size());
  for (const auto& e : extents) {
    starts_.push_back(e.first);
    lengths_.push_back(e.second);
  }
}