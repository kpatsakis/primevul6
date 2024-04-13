bool TensorSlice::operator==(const TensorSlice& other) const {
  return dims() == other.dims() && starts_ == other.starts_ &&
         lengths_ == other.lengths_;
}