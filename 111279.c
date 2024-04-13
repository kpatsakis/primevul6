int64_t TensorSlice::GetExtentLength(const TensorSliceProto::Extent& extent) {
  if (!HasExtentLength(extent)) return -1;
  return extent.length();
}