  void CopyReshapeWithInput(const NodeDef* reshape, NodeDef* new_reshape,
                            const string& input, const string& shape) {
    new_reshape->set_op("Reshape");
    new_reshape->set_device(reshape->device());
    SetDataTypeToAttr(GetDataTypeFromAttr(*reshape, "T"), "T", new_reshape);
    SetDataTypeToAttr(GetDataTypeFromAttr(*reshape, "Tshape"), "Tshape",
                      new_reshape);

    new_reshape->add_input(input);
    ctx().node_map->AddOutput(NodeName(input), new_reshape->name());
    new_reshape->add_input(shape);
    ctx().node_map->AddOutput(NodeName(shape), new_reshape->name());

    AddToOptimizationQueue(new_reshape);
  }