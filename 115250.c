int32_t ByteArray::SetFilledLength(int32_t filled_length) {
  filled_length_ = std::min<int32_t>(filled_length, storage_length_);
  return filled_length_;
}