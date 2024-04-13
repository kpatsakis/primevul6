Status InferenceContext::ExpandOutputs(int new_output_size) {
  const int outputs_size = outputs_.size();
  if (new_output_size < outputs_size) {
    return errors::InvalidArgument("Trying to reduce number of outputs of op.");
  }
  outputs_.resize(new_output_size, nullptr);
  output_handle_shapes_and_types_.resize(new_output_size);
  return Status::OK();
}