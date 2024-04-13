Status InferenceContext::input(StringPiece input_name,
                               std::vector<ShapeHandle>* output) const {
  const auto result = input_name_map_.find(input_name);
  if (result == input_name_map_.end()) {
    return errors::InvalidArgument("Unknown input name: ", input_name);
  } else {
    output->clear();
    for (int i = result->second.first; i < result->second.second; ++i) {
      output->push_back(inputs_[i]);
    }
  }
  return Status::OK();
}