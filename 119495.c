  Status CreateOptimizedNodesGroup(NodeDef* root_node,
                                   OptimizedNodesGroup* group) const {
    const OpInfo::TensorProperties* root_node_output_properties;
    TF_RETURN_IF_ERROR(
        GetTensorProperties(root_node->name(), &root_node_output_properties));

    group->root_node = root_node;
    group->root_shape = root_node_output_properties->shape();

    group->optimized_nodes.reserve(root_node->input_size());
    for (int i = 0; i < root_node->input_size(); ++i) {
      const string& input_i = root_node->input(i);
      // TODO(ezhulenev): add support for control inputs
      if (IsControlInput(input_i)) continue;
      TF_RETURN_IF_ERROR(AbsorbInputByOptimizedNodesGroup(input_i, group));
    }

    return Status::OK();
  }