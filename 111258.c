bool TensorSlice::IsFull() const {
  for (int d = 0; d < dims(); ++d) {
    if (!IsFullAt(d)) return false;
  }
  return true;
}