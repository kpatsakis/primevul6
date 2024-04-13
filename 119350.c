  InputAndShape AddAggregatedInputs(const NodeDef& root_node,
                                    const string& node_name,
                                    const InputAndShape& left,
                                    const InputAndShape& right) {
    // copy attributes from a root node
    DataType dtype = root_node.attr().at("T").type();

    // add new Add node
    NodeDef* node = AddEmptyNode(node_name);
    node->set_op((dtype == DT_STRING || dtype == DT_STRING_REF) ? "Add"
                                                                : "AddV2");
    node->set_device(root_node.device());
    (*node->mutable_attr())["T"].set_type(dtype);
    node->add_input(left.input);
    node->add_input(right.input);

    ctx().node_map->AddOutput(left.input, node_name);
    ctx().node_map->AddOutput(right.input, node_name);

    MarkWithTag(kAddOpsRewriteTag, node);
    return InputAndShape(
        node_name, TensorShapeProto());  // shape is not important at this point
  }