inline bool InitTensorDataForReduce(const int* dims, const int num_dims,
                                    const T init_value, T* data) {
  size_t num_elements = 1;
  for (int idx = 0; idx < num_dims; ++idx) {
    size_t current = static_cast<size_t>(dims[idx]);
    // Overflow prevention.
    if (num_elements > std::numeric_limits<size_t>::max() / current) {
      return false;
    }
    num_elements *= current;
  }
  for (size_t idx = 0; idx < num_elements; ++idx) {
    data[idx] = init_value;
  }
  return true;
}