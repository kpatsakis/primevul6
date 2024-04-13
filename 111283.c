  ~TensorSliceReaderTable() override {
    delete table_;
    delete file_;
  }