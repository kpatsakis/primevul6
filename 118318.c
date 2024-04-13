int32_t FontData::Size() const {
  return std::min<int32_t>(array_->Size() - bound_offset_, bound_length_);
}