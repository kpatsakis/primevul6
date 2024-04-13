Status InferenceContext::set_output(StringPiece output_name,
                                    const std::vector<ShapeHandle>& shapes) {
  auto result = output_name_map_.find(output_name);
  if (result == output_name_map_.end()) {
    return errors::InvalidArgument("Unknown output name: ", output_name);
  } else {
    const int start = result->second.first;
    const int size = result->second.second - start;
    const int shapes_size = shapes.size();
    if (size != shapes_size) {
      return errors::InvalidArgument("Must have exactly ", shapes.size(),
                                     " shapes.");
    }
    for (int i = 0; i < shapes_size; ++i) {
      outputs_[i + start] = shapes[i];
    }
  }
  return Status::OK();
}