TensorSliceReader::TensorSliceReader(const string& filepattern,
                                     OpenTableFunction open_function)
    : TensorSliceReader(filepattern, std::move(open_function), kLoadAllShards) {
}