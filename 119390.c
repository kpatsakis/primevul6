  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    const NodeScopeAndName matmul = ParseNodeScopeAndName(node->name());
    const string optimized_node_name = OptimizedNodeName(matmul);
    if (ctx().node_map->NodeExists(optimized_node_name)) return Status::OK();

    NodeDef* a;
    NodeDef* b;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &a));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &b));

    bool is_complex = false;
    if (node->op() != "SparseMatMul") {
      const DataType type = GetDataTypeFromAttr(*node, "T");
      is_complex = (type == DT_COMPLEX64) || (type == DT_COMPLEX128);
    }

    const std::set<string> foldable_transpose_ops =
        !is_complex
            ? std::set<string>{"ConjugateTranspose", "Transpose"}
            : (IsAnyBatchMatMul(*node) ? std::set<string>{"ConjugateTranspose"}
                                       : std::set<string>{"Transpose"});

    const bool a_is_foldable = foldable_transpose_ops.count(a->op()) > 0 &&
                               IsInnerMatrixTransposeNode(*a, ctx().node_map);
    const bool b_is_foldable = foldable_transpose_ops.count(b->op()) > 0 &&
                               IsInnerMatrixTransposeNode(*b, ctx().node_map);
    if (!a_is_foldable && !b_is_foldable) return Status::OK();

    NodeDef* new_op = AddCopyNode(optimized_node_name, node);

    if (a_is_foldable) {
      const string attr_a = IsAnyBatchMatMul(*node) ? "adj_x" : "transpose_a";
      FlipBooleanAttr(attr_a, new_op);
      new_op->set_input(0, a->input(0));
      ctx().node_map->UpdateInput(new_op->name(), a->name(), a->input(0));
    } else {
      ctx().node_map->UpdateOutput(a->name(), node->name(), new_op->name());
    }

    if (b_is_foldable) {
      const string attr_b = IsAnyBatchMatMul(*node) ? "adj_y" : "transpose_b";
      FlipBooleanAttr(attr_b, new_op);
      new_op->set_input(1, b->input(0));
      ctx().node_map->UpdateInput(new_op->name(), b->name(), b->input(0));
    } else {
      ctx().node_map->UpdateOutput(b->name(), node->name(), new_op->name());
    }

    std::vector<const NodeDef*> deps_to_forward = {node};
    if (a_is_foldable) deps_to_forward.push_back(a);
    if (b_is_foldable) deps_to_forward.push_back(b);
    ForwardControlDependencies(new_op, deps_to_forward);
    *simplified_node_name = new_op->name();

    return Status::OK();
  }