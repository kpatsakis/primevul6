void InferenceContext::PreInputInit(
    const OpDef& op_def, const std::vector<const Tensor*>& input_tensors,
    const std::vector<ShapeHandle>& input_tensors_as_shapes) {
  // TODO(mdan): This is also done at graph construction. Run only here instead?
  const auto ret = full_type::SpecializeType(attrs_, op_def);
  if (!ret.status().ok()) {
    construction_status_ = ret.status();
    return;
  }
  ret_types_ = ret.ValueOrDie();

  input_tensors_ = input_tensors;
  input_tensors_as_shapes_ = input_tensors_as_shapes;

  construction_status_ =
      NameRangesForNode(attrs_, op_def, &input_name_map_, &output_name_map_);
  if (!construction_status_.ok()) return;

  int num_outputs = 0;
  for (const auto& e : output_name_map_) {
    num_outputs = std::max(num_outputs, e.second.second);
  }
  outputs_.assign(num_outputs, nullptr);
  output_handle_shapes_and_types_.resize(num_outputs);
}