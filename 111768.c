InferenceContext::InferenceContext(
    int graph_def_version, const AttrSlice& attrs, const OpDef& op_def,
    const std::vector<ShapeHandle>& input_shapes,
    const std::vector<const Tensor*>& input_tensors,
    const std::vector<ShapeHandle>& input_tensors_as_shapes,
    std::vector<std::unique_ptr<std::vector<ShapeAndType>>>
        input_handle_shapes_and_types)
    : graph_def_version_(graph_def_version), attrs_(attrs) {
  PreInputInit(op_def, input_tensors, input_tensors_as_shapes);
  if (!construction_status_.ok()) return;
  inputs_ = input_shapes;

  PostInputInit(std::move(input_handle_shapes_and_types));
}