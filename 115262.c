void ByteArray::Init(int32_t filled_length,
                     int32_t storage_length,
                     bool growable) {
  storage_length_ = storage_length;
  growable_ = growable;
  SetFilledLength(filled_length);
}