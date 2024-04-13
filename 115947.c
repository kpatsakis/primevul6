  explicit BasicMemoryWriter(const Allocator& alloc = Allocator())
    : BasicWriter<Char>(buffer_), buffer_(alloc) {}