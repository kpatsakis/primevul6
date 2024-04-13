  Status RewriteGraph(const NodeDef* node, const NodeDef* pack,
                      int64 slice_start_value, int pack_axis,
                      bool must_expand_dims, string* simplified_node_name) {
    const string& input_slice = pack->input(slice_start_value);

    const OpInfo::TensorProperties* input_slice_properties;
    TF_RETURN_IF_ERROR(GetTensorProperties(pack->input(slice_start_value),
                                           &input_slice_properties));
    PartialTensorShape input_slice_shape(input_slice_properties->shape());

    const OpInfo::TensorProperties* output_properties;
    TF_RETURN_IF_ERROR(GetTensorProperties(
        strings::StrCat(node->name(), ":", 0), &output_properties));
    PartialTensorShape output_shape(output_properties->shape());
    NodeDef* output =
        AddEmptyNode(OptimizedNodeName(ParseNodeScopeAndName(node->name())));
    if (!must_expand_dims) {
      output->set_op("Identity");
      output->set_device(node->device());
      SetDataTypeToAttr(output_properties->dtype(), "T", output);
      output->add_input(input_slice);
    } else {
      NodeDef* axis = AddEmptyNode(
          OptimizedNodeName(ParseNodeScopeAndName(node->name()), "Axis"));
      axis->set_op("Const");
      axis->set_device(node->device());
      // We need to add a control edge from input slice to guarantee that axis
      // constant will be executed in the same frame as `input_slice`, otherwise
      // ExpandDims might have mismatched input frames.
      axis->add_input(absl::StrCat("^", ParseTensorName(input_slice).node()));
      auto axis_attr = axis->mutable_attr();
      SetDataTypeToAttr(DT_INT32, "dtype", axis);
      auto* axis_t = (*axis_attr)["value"].mutable_tensor();
      axis_t->set_dtype(DT_INT32);
      axis_t->add_int_val(pack_axis);
      AddToOptimizationQueue(axis);
      output->set_op("ExpandDims");
      output->set_device(node->device());
      SetDataTypeToAttr(output_properties->dtype(), "T", output);
      SetDataTypeToAttr(DT_INT32, "Tdim", output);
      output->add_input(input_slice);
      output->add_input(axis->name());
    }

    // Copy dependencies over.
    ForwardControlDependencies(output, {node, pack});
    AddToOptimizationQueue(output);
    *simplified_node_name = output->name();

    return Status::OK();
  }