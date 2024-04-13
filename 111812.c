Status InferenceContext::GetScalarFromTensor(const Tensor* t, int64_t idx,
                                             int64_t* val) {
  // Caller must ensure that <t> is not NULL.
  const int rank = t->dims();
  if (rank != 1) {
    return errors::InvalidArgument("Input must be 1D but has rank ", rank);
  }

  if (t->dtype() == DataType::DT_INT32) {
    auto flat_t = t->flat<int32>();
    if (idx < 0 || idx >= flat_t.size()) {
      return errors::InvalidArgument("Invalid index ", idx,
                                     " for Tensor of size ", flat_t.size());
    }
    *val = flat_t(idx);
    return Status::OK();
  } else if (t->dtype() == DataType::DT_INT64) {
    auto flat_t = t->flat<int64_t>();
    if (idx < 0 || idx >= flat_t.size()) {
      return errors::InvalidArgument("Invalid index ", idx,
                                     " for Tensor of size ", flat_t.size());
    }
    *val = flat_t(idx);
    return Status::OK();
  } else {
    return errors::InvalidArgument("Tensor input must be int32 or int64.");
  }
}