Status InferenceContext::output(StringPiece output_name,
                                std::vector<ShapeHandle>* output) const {
  const auto result = output_name_map_.find(output_name);
  if (result == output_name_map_.end()) {
    return errors::InvalidArgument("Unknown output name: ", output_name);
  } else {
    output->clear();
    for (int i = result->second.first; i < result->second.second; ++i) {
      output->push_back(outputs_[i]);
    }
  }
  return Status::OK();
}