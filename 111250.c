TensorSlice::TensorSlice(const TensorSliceProto& proto) {
  starts_.reserve(proto.extent_size());
  lengths_.reserve(proto.extent_size());
  for (const auto& e : proto.extent()) {
    starts_.push_back(e.start());
    lengths_.push_back(GetExtentLength(e));
  }
}