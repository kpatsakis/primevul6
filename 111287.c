bool TensorSlice::Intersect(const TensorSlice& other,
                            TensorSlice* result) const {
  // First, if two slices have different ranks, they obviously don't overlap
  // -- in fact they are not compatible.
  if (dims() != other.dims()) {
    return false;
  }

  // Setting the result to the right dimension
  if (result) {
    result->SetFullSlice(dims());
  }
  // The two slices overlap if they overlap in all dimensions.
  for (int d = 0; d < dims(); ++d) {
    if (IsFullAt(d)) {
      if (result) {
        result->set_start(d, other.start(d));
        result->set_length(d, other.length(d));
      }
    } else if (other.IsFullAt(d)) {
      if (result) {
        result->set_start(d, start(d));
        result->set_length(d, length(d));
      }
    } else {
      // If we have an intersection here, it should have a start that is the
      // max of the two starts and an end that is the min of the two ends.
      int64_t s = std::max(start(d), other.start(d));
      int64_t l = std::min(end(d), other.end(d)) - s;
      if (l > 0) {
        // We have a real intersection
        if (result) {
          result->set_start(d, s);
          result->set_length(d, l);
        }
      } else {
        // We don't have an intersection for this dimension -- thus we don't
        // have any intersection at all.
        if (result) {
          result->Clear();
        }
        return false;
      }
    }
  }
  // If we are here, we know there is overlap in every dimension.
  return true;
}