void TensorSlice::AsProto(TensorSliceProto* proto) const {
  for (int d = 0; d < dims(); ++d) {
    TensorSliceProto::Extent* e = proto->add_extent();
    // We only need to record the explicit slice for non-full slices
    if (!IsFullAt(d)) {
      e->set_start(starts_[d]);
      e->set_length(lengths_[d]);
    }
  }
}