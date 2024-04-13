  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    const NodeScopeAndName mul = ParseNodeScopeAndName(node->name());
    const string optimized_node_name = OptimizedNodeName(mul);
    if (ctx().node_map->NodeExists(optimized_node_name)) return Status::OK();

    const DataType type = GetDataTypeFromAttr(*node, "T");
    bool is_complex = (type == DT_COMPLEX64) || (type == DT_COMPLEX128);

    if (!is_complex || NodeIsOnCpu(*node)) {
      NodeDef* new_square_node = AddCopyNode(optimized_node_name, node);
      new_square_node->set_op("Square");
      for (int i = 1; i < new_square_node->input_size(); ++i) {
        new_square_node->set_input(i - 1, new_square_node->input(i));
      }
      new_square_node->mutable_input()->RemoveLast();
      for (const string& input : new_square_node->input()) {
        ctx().node_map->AddOutput(NodeName(input), new_square_node->name());
      }
      *simplified_node_name = new_square_node->name();
    }

    return Status::OK();
  }