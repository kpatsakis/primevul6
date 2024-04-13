  StatusOr<DeviceMemory<uint8>> AllocateBytes(int64 byte_size) override {
    output_allocated = true;
    DCHECK(total_byte_size_ == 0)
        << "Reserve space allocator can only be called once";
    int64 allocate_count =
        Eigen::divup(byte_size, static_cast<int64>(sizeof(T)));

    Tensor* temporary_memory = nullptr;
    Status allocation_status(context_->allocate_output(
        output_index_, TensorShape({allocate_count}), &temporary_memory));
    if (!allocation_status.ok()) {
      return allocation_status;
    }
    total_byte_size_ += byte_size;
    auto memory_uint8 = DeviceMemory<uint8>::MakeFromByteSize(
        temporary_memory->template flat<T>().data(),
        temporary_memory->template flat<T>().size() * sizeof(T));
    return StatusOr<DeviceMemory<uint8>>(memory_uint8);
  }