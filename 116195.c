  explicit MemoryBuffer(const Allocator &alloc = Allocator())
      : Allocator(alloc), Buffer<T>(data_, SIZE) {}