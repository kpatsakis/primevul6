void GetSizeSplitsVector(const TfLiteTensor* size_splits,
                         std::vector<int64_t>* size_splits_vector) {
  const auto num_elements = NumElements(size_splits);
  for (int i = 0; i < num_elements; ++i) {
    size_splits_vector->push_back(GetTensorData<T>(size_splits)[i]);
  }
}