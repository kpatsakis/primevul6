void TensorSlice::SetFullSlice(int dim) {
  Clear();
  starts_.reserve(dim);
  lengths_.reserve(dim);
  for (int d = 0; d < dim; ++d) {
    starts_.push_back(0);
    lengths_.push_back(kFullExtent);
  }
}