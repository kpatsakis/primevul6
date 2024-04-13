  Status TrySimplify(NodeDef* reduction_node,
                     string* simplified_node_name) override {
    if (IsInPreserveSet(*reduction_node)) return Status::OK();

    bool optimized = false;

    // Inputs 1 (indices) and 2 (segment_ids) can be either DT_INT32 or
    // DT_INT64.
    std::array<std::pair<int, string>, 2> input_details = {
        std::make_pair(1, "Tidx"), std::make_pair(2, "Tsegmentids")};

    for (const auto& input : input_details) {
      int input_index = input.first;
      const string& type_attr_name = input.second;
      NodeDef* cast_node = nullptr;
      TF_RETURN_IF_ERROR(
          GetInputNode(reduction_node->input(input_index), &cast_node));
      DataType original_index_type;
      if (IsCastFromSupportedType(*cast_node, &original_index_type)) {
        reduction_node->set_input(input_index, cast_node->input(0));
        ctx().node_map->UpdateInput(reduction_node->name(),
                                    reduction_node->input(1),
                                    cast_node->input(0));
        SetDataTypeToAttr(original_index_type, type_attr_name, reduction_node);
        optimized = true;
      }
    }

    if (optimized) *simplified_node_name = reduction_node->name();
    return Status::OK();
  }