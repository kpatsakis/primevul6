void FontData::Init(ByteArray* ba) {
  array_ = ba;
  bound_offset_ = 0;
  bound_length_ = GROWABLE_SIZE;
}