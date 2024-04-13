Status OpenTableTensorSliceReader(const string& fname,
                                  TensorSliceReader::Table** result) {
  *result = nullptr;
  Env* env = Env::Default();
  std::unique_ptr<RandomAccessFile> f;
  Status s = env->NewRandomAccessFile(fname, &f);
  if (s.ok()) {
    uint64 file_size;
    s = env->GetFileSize(fname, &file_size);
    if (s.ok()) {
      table::Options options;
      table::Table* table;
      s = table::Table::Open(options, f.get(), file_size, &table);
      if (s.ok()) {
        *result = new TensorSliceReaderTable(f.release(), table);
        return Status::OK();
      } else {
        s = Status(s.code(),
                   strings::StrCat(s.error_message(),
                                   ": perhaps your file is in a different "
                                   "file format and you need to use a "
                                   "different restore operator?"));
      }
    }
  }
  LOG(WARNING) << "Could not open " << fname << ": " << s;
  return s;
}