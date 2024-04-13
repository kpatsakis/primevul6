FontData::FontData(FontData* data, int32_t offset, int32_t length) {
  Init(data->array_);
  Bound(data->bound_offset_ + offset, length);
}