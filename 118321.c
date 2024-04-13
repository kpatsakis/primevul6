int32_t FontData::Length() const {
  return std::min<int32_t>(array_->Length() - bound_offset_, bound_length_);
}