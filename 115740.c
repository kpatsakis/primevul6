  StatusOr<DeviceMemory<uint8>> AllocateBytes(int64 byte_size) override {
    Tensor temporary_memory;
    const DataType tf_data_type = DataTypeToEnum<T>::v();
    int64 allocate_count =
        Eigen::divup(byte_size, static_cast<int64>(sizeof(T)));
    Status allocation_status(context_->allocate_temp(
        tf_data_type, TensorShape({allocate_count}), &temporary_memory));
    if (!allocation_status.ok()) {
      return allocation_status;
    }
    // Hold the reference of the allocated tensors until the end of the
    // allocator.
    allocated_tensors_.push_back(temporary_memory);
    total_byte_size_ += byte_size;
    return DeviceMemory<uint8>::MakeFromByteSize(
        temporary_memory.template flat<T>().data(),
        temporary_memory.template flat<T>().size() * sizeof(T));
  }