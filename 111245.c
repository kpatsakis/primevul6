Status TensorSlice::SliceTensorShape(const TensorShape& shape,
                                     TensorShape* result_shape) const {
  result_shape->Clear();
  // Mismatching ranks: we can't apply the slice at all.
  if (shape.dims() != dims()) {
    return errors::Internal("Mismatching ranks: shape = ", shape.DebugString(),
                            ", slice = ", DebugString());
  }
  for (int d = 0; d < dims(); ++d) {
    if (IsFullAt(d)) {
      result_shape->AddDim(shape.dim_size(d));
    } else {
      // Check if the extent applies to the dimension
      if (end(d) <= shape.dim_size(d)) {
        // Yes: the end is within the range of the dim -- we adjust the result
        // shape so that its size along this dimension is the length of the
        // slice.
        result_shape->AddDim(length(d));
      } else {
        // The extent doesn't apply to the dimension
        result_shape->Clear();
        return errors::Internal("Extent in dimension ", d,
                                " out of bounds: shape = ", shape.DebugString(),
                                ", slice = ", DebugString());
      }
    }
  }
  // If we are here, we have successfully applied the shape.
  return Status::OK();
}