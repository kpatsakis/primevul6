void InferenceContext::ShapeHandleToProto(ShapeHandle handle,
                                          TensorShapeProto* proto) {
  if (!RankKnown(handle)) {
    proto->set_unknown_rank(true);
    return;
  }

  for (int32_t i = 0; i < Rank(handle); ++i) {
    DimensionHandle dim = Dim(handle, i);
    auto* dim_shape = proto->add_dim();
    if (ValueKnown(dim)) {
      dim_shape->set_size(Value(dim));
    } else {
      dim_shape->set_size(-1);
    }
  }
}