Status TensorShapeFromTensor(const Tensor& t, PartialTensorShape* out) {
  if (t.shape() == TensorShape({})) {
    if ((t.dtype() == DT_INT32 && t.scalar<int32>()() == -1) ||
        (t.dtype() == DT_INT64 && t.scalar<int64>()() == -1)) {
      *out = PartialTensorShape();
      return Status::OK();
    }
    return errors::InvalidArgument(
        "The only valid scalar shape tensor is the fully unknown shape "
        "specified as -1.");
  }
  if (t.dtype() == DT_INT32) {
    return PartialTensorShape::MakePartialShape(t.vec<int32>().data(),
                                                t.NumElements(), out);
  } else if (t.dtype() == DT_INT64) {
    return PartialTensorShape::MakePartialShape(t.vec<int64>().data(),
                                                t.NumElements(), out);
  }
  return errors::InvalidArgument(
      "Expected an int32 or int64 shape tensor; found ",
      DataTypeString(t.dtype()));
}