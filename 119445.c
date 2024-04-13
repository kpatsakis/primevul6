  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(EnsureNodeIsSupported(node));

    // Bypass Bitcast whose source type and destination type are equal.
    AttrSlice attrs(*node);
    DataType input_type;
    TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "T", &input_type));
    DataType output_type;
    TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "type", &output_type));
    if ((input_type == output_type) && !IsInPreserveSet(*node)) {
      *simplified_node_name = node->input(0);
      return Status::OK();
    }

    NodeDef* bitcast;
    TF_RETURN_IF_ERROR(GetInputNode(node->name(), &bitcast));
    NodeDef* operand;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &operand));

    if (IsBitcast(*operand) && !IsInPreserveSet(*operand)) {
      AttrSlice operand_attrs(*operand);
      DataType operand_input_type;
      TF_RETURN_IF_ERROR(GetNodeAttr(operand_attrs, "T", &operand_input_type));
      // Bitcast(Bitcast(x, type1), type2) => Bitcast(x, type2)
      bitcast->set_input(0, operand->input(0));
      SetDataTypeToAttr(operand_input_type, "T", bitcast);
      ctx().node_map->UpdateInput(bitcast->name(), bitcast->input(0),
                                  operand->input(0));
      AddToOptimizationQueue(bitcast);
      *simplified_node_name = bitcast->name();
    }

    return Status::OK();
  }