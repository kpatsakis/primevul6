void TensorSlice::UpdateToCover(const TensorSlice& other) {
  DCHECK_EQ(dims(), other.dims());
  for (int d = 0; d < dims(); ++d) {
    if (!IsFullAt(d)) {
      if (other.IsFullAt(d)) {
        starts_[d] = 0;
        lengths_[d] = kFullExtent;
      } else {
        const auto new_end = std::max(end(d), other.end(d));
        set_start(d, std::min(start(d), other.start(d)));
        set_length(d, new_end - start(d));
      }
    }
  }
}