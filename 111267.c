bool TensorSlice::HasExtentLength(const TensorSliceProto::Extent& extent) {
  return extent.has_length_case() == TensorSliceProto::Extent::kLength;
}