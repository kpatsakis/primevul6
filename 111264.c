TensorSliceReader::TensorSliceReader(const string& filepattern)
    : TensorSliceReader(filepattern, OpenTableTensorSliceReader,
                        kLoadAllShards) {}