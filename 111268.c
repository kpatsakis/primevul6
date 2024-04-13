  explicit TensorSliceReaderTable(RandomAccessFile* f, table::Table* t)
      : file_(f), table_(t) {}