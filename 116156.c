  BasicMemoryWriter(BasicMemoryWriter &&other)
    : BasicWriter<Char>(buffer_), buffer_(std::move(other.buffer_)) {
  }