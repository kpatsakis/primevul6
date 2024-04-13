  InputAndShape AddInputsOfSymbolicallyEqualShape(
      const NodeDef& root_node, const string& node_name,
      const std::vector<InputAndShape>& inputs) {
    CHECK(!inputs.empty()) << "Inputs must be non-empty";

    // Do not create redundant AddN nodes
    if (inputs.size() == 1 || root_node.attr().count("T") == 0) {
      return inputs[0];
    }

    // get shape from representative element
    auto shape = inputs[0].shape;

    // copy attributes from a root node
    DataType dtype = root_node.attr().at("T").type();

    // add new AddN node
    NodeDef* node = AddEmptyNode(node_name);
    node->set_op("AddN");
    node->set_device(root_node.device());
    (*node->mutable_attr())["T"].set_type(dtype);
    (*node->mutable_attr())["N"].set_i(inputs.size());

    for (const auto& inputAndShape : inputs) {
      ctx().node_map->AddOutput(inputAndShape.input, node_name);
      node->add_input(inputAndShape.input);
    }

    MarkWithTag(kAddOpsRewriteTag, node);
    return InputAndShape(node_name, shape);
  }