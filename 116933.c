  PyTapeTensor(int64_t id, tensorflow::DataType dtype,
               const tensorflow::TensorShape& shape)
      : id_(id), dtype_(dtype), shape_(shape) {}