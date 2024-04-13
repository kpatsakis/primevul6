  Status TrySimplify(NodeDef* reduction_node,
                     string* simplified_node_name) override {
    if (IsInPreserveSet(*reduction_node)) return Status::OK();

    // Input 0 (data) of the reduction node must be a tf.gather() on the 0th
    // axis.
    NodeDef* gather_node = nullptr;
    TF_RETURN_IF_ERROR(GetInputNode(reduction_node->input(0), &gather_node));
    if (!IsGather(*gather_node) || IsInPreserveSet(*gather_node) ||
        gather_node->device() != reduction_node->device())
      return Status::OK();
    if (gather_node->op() == "GatherV2" && !IsAxis0(*gather_node, 2))
      return Status::OK();

    // Input 1 (indices) of the gather node must be a tf.unique() on the 0th
    // axis.
    NodeDef* unique_node = nullptr;
    TF_RETURN_IF_ERROR(GetInputNode(gather_node->input(1), &unique_node));
    if (!IsUnique(*unique_node) || IsInPreserveSet(*unique_node) ||
        unique_node->device() != gather_node->device())
      return Status::OK();
    if (unique_node->op() == "UniqueV2" && !IsAxis0(*unique_node, 1))
      return Status::OK();

    DataType unique_element_type;
    TF_RETURN_IF_ERROR(GetNodeAttr(*unique_node, "T", &unique_element_type));

    // Input 1 (indices) of the reduction node must be output 1 of the unique
    // node.
    const TensorId idx_tensor = ParseTensorName(reduction_node->input(1));
    if (idx_tensor != TensorId(unique_node->name(), 1)) return Status::OK();

    // Input 0 (data) of the reduction node becomes input 1 (params) of the
    // gather node.
    reduction_node->set_input(0, gather_node->input(0));
    ctx().node_map->UpdateInput(reduction_node->name(),
                                reduction_node->input(0),
                                gather_node->input(0));

    // Input 1 (indices) of the reduction node becomes input 0 (x) of the unique
    // node.
    reduction_node->set_input(1, unique_node->input(0));
    ctx().node_map->UpdateInput(reduction_node->name(),
                                reduction_node->input(1),
                                unique_node->input(0));
    SetDataTypeToAttr(unique_element_type, "Tidx", reduction_node);

    *simplified_node_name = reduction_node->name();
    return Status::OK();
  }