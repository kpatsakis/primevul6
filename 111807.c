Status InferenceContext::GetScalarFromTensor(const Tensor* t, int64_t* val) {
  // Caller must ensure that <t> is not NULL.
  const int rank = t->dims();
  if (rank != 0) {
    return errors::InvalidArgument("Input must be scalar but has rank ", rank);
  }

  if (t->dtype() == DataType::DT_INT32) {
    *val = t->scalar<int32>()();
    return Status::OK();
  } else if (t->dtype() == DataType::DT_INT64) {
    *val = t->scalar<int64_t>()();
    return Status::OK();
  } else {
    return errors::InvalidArgument("Scalar input must be int32 or int64.");
  }
}