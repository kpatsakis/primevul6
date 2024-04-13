Status InferenceContext::MakeDimForScalarInputWithNegativeIndexing(
    int idx, int input_rank, DimensionHandle* out) {
  int64_t val;
  const Tensor* t = input_tensor(idx);
  if (t == nullptr) {
    *out = UnknownDim();
    return Status::OK();
  }
  TF_RETURN_IF_ERROR(GetScalarFromTensor(t, &val));
  if (val < 0) {
    if (input_rank < 0) {
      *out = UnknownDim();
      return Status::OK();
    } else if (val + input_rank < 0) {
      return errors::InvalidArgument("Dimension size, given by scalar input ",
                                     val, " must be in range [-", input_rank,
                                     ", ", input_rank, ")");
    } else {
      val += input_rank;
    }
  } else if (input_rank >= 0 && val >= input_rank) {
    return errors::InvalidArgument("Dimension size, given by scalar input ",
                                   val, " must be in range [-", input_rank,
                                   ", ", input_rank, ")");
  }
  *out = MakeDim(val);
  return Status::OK();
}