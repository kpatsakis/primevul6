Status InferenceContext::MakeDimForScalarInput(int idx, DimensionHandle* out) {
  int64_t val;
  const Tensor* t = input_tensor(idx);
  if (t == nullptr) {
    *out = UnknownDim();
    return Status::OK();
  }
  TF_RETURN_IF_ERROR(GetScalarFromTensor(t, &val));
  if (val < 0) {
    return errors::InvalidArgument("Dimension size, given by scalar input ",
                                   idx, ", must be non-negative but is ", val);
  }
  *out = MakeDim(val);
  return Status::OK();
}