TensorSliceReader::~TensorSliceReader() {
  for (auto& temp : tensors_) {
    delete temp.second;
  }
  tensors_.clear();
}