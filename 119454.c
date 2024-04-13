  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    // *node is a StridedSlice NodeDef.
    NodeDef* pack;

    // Get the input and see if it's a Pack op.
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &pack));
    if (!IsPack(*pack)) return Status::OK();

    bool return_early;
    PartialTensorShape pack_output_shape;
    int pack_axis;
    TF_RETURN_IF_ERROR(
        CheckInputs(node, pack, &pack_output_shape, &pack_axis, &return_early));
    if (return_early) return Status::OK();

    int64 slice_start_value;
    bool found;
    bool must_expand_dims;
    TF_RETURN_IF_ERROR(GetSliceAxis(node, pack, pack_output_shape, pack_axis,
                                    &slice_start_value, &found,
                                    &must_expand_dims));
    if (!found) return Status::OK();

    return RewriteGraph(node, pack, slice_start_value, pack_axis,
                        must_expand_dims, simplified_node_name);
  }