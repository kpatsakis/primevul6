void TensorSlice::Extend(int dim) {
  int old_dim = dims();
  DCHECK_LE(old_dim, dim);
  starts_.resize(dim);
  lengths_.resize(dim);
  for (int d = old_dim; d < dim; ++d) {
    starts_[d] = 0;
    lengths_[d] = kFullExtent;
  }
}