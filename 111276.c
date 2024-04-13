void TensorSlice::ComputeRelative(const TensorSlice& sub,
                                  TensorSlice* relative) const {
  DCHECK_EQ(dims(), sub.dims());
  relative->SetFullSlice(dims());
  for (int d = 0; d < dims(); ++d) {
    if (IsFullAt(d)) {
      relative->set_start(d, sub.start(d));
      relative->set_length(d, sub.length(d));
    } else {
      // Otherwise the relative start is the difference between the start of
      // sub and the start of base
      relative->set_start(d, sub.start(d) - start(d));
      relative->set_length(d, sub.length(d));
    }
  }
}