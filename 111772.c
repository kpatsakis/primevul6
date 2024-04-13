void InferenceContext::PostInputInit(
    std::vector<std::unique_ptr<std::vector<ShapeAndType>>> input_handle_data) {
  int num_inputs_from_node_def = 0;
  for (const auto& e : input_name_map_) {
    num_inputs_from_node_def =
        std::max(num_inputs_from_node_def, e.second.second);
  }

  // Allow passing empty shapes/dtypes to avoid changing every single test.
  if (input_handle_data.empty()) {
    input_handle_shapes_and_types_.resize(inputs_.size());
  } else {
    if (input_handle_data.size() != inputs_.size()) {
      construction_status_ = errors::InvalidArgument(
          "Wrong number of handle shapes passed; expected ", inputs_.size(),
          " got ", input_handle_data.size());
      return;
    }
    input_handle_shapes_and_types_ = std::move(input_handle_data);
  }
  const int inputs_size = inputs_.size();
  if (inputs_size != num_inputs_from_node_def) {
    construction_status_ = errors::InvalidArgument(
        "Wrong number of inputs passed: ", inputs_.size(), " while ",
        num_inputs_from_node_def, " expected based on NodeDef");
    return;
  }

  CHECK_LE(input_tensors_.size(), inputs_.size());
  input_tensors_.resize(inputs_.size());
  requested_input_tensor_.resize(inputs_.size());
  requested_input_tensor_as_partial_shape_.resize(inputs_.size());
}