gtl::InlinedVector<INDICES_TYPE, 8> GetFlattenedRelativeOffsets(
    INDICES_TYPE small_stride, INDICES_TYPE big_stride) {
  gtl::InlinedVector<INDICES_TYPE, 8> flattened_offsets(small_stride);
  for (auto i = 0; i < small_stride; i++) {
    flattened_offsets[i] = i * big_stride;
  }
  return flattened_offsets;
}