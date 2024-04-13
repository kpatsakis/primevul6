  NodeDef* CopyReshapeAndInsertAfter(const NodeDef* reshape, NodeDef* unary,
                                     const string& new_reshape_name) {
    // Copy the attributes of the original reshape
    NodeDef* new_reshape = AddEmptyNode(new_reshape_name);
    new_reshape->set_op("Reshape");
    new_reshape->set_device(reshape->device());
    SetDataTypeToAttr(GetDataTypeFromAttr(*reshape, "T"), "T", new_reshape);
    SetDataTypeToAttr(GetDataTypeFromAttr(*reshape, "Tshape"), "Tshape",
                      new_reshape);

    // Forward the consumers of unary to reshape
    UpdateConsumers(unary, new_reshape->name());

    // Add unary and the original shape as inputs
    new_reshape->add_input(unary->name());
    ctx().node_map->AddOutput(unary->name(), new_reshape->name());
    new_reshape->add_input(reshape->input(1));
    ctx().node_map->AddOutput(reshape->input(1), new_reshape->name());

    return new_reshape;
  }