Status InferenceContext::Merge(DimensionHandle d0, DimensionHandle d1,
                               DimensionHandle* out) {
  if (d0.SameHandle(d1)) {
    *out = d0;
    return Status::OK();
  } else if (!ValueKnown(d1)) {
    *out = d0;
    merged_dims_.emplace_back(d0, d1);
    return Status::OK();
  } else if (!ValueKnown(d0)) {
    *out = d1;
    merged_dims_.emplace_back(d0, d1);
    return Status::OK();
  } else if (Value(d0) == Value(d1)) {
    *out = d0;
    return Status::OK();
  } else {
    *out = nullptr;
    return errors::InvalidArgument("Dimensions must be equal, but are ",
                                   Value(d0), " and ", Value(d1));
  }
}