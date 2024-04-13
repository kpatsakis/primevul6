void FontData::Bound(int32_t offset, int32_t length) {
  // Inputs should not be negative.
  CHECK(offset >= 0);
  CHECK(length >= 0);

  // Check to make sure |bound_offset_| will not overflow.
  CHECK(bound_offset_ <= std::numeric_limits<int32_t>::max() - offset);
  const int32_t new_offset = bound_offset_ + offset;

  if (length == GROWABLE_SIZE) {
    // When |length| has the special value of GROWABLE_SIZE, it means the size
    // should not have any artificial limits, thus it is just the underlying
    // |array_|'s size. Just make sure |new_offset| is still within bounds.
    CHECK(new_offset <= array_->Size());
  } else {
    // When |length| has any other value, |new_offset| + |length| points to the
    // end of the array. Make sure that is within bounds, but use subtraction to
    // avoid an integer overflow.
    CHECK(new_offset <= array_->Size() - length);
  }

  bound_offset_ = new_offset;
  bound_length_ = length;
}