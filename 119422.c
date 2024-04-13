  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    const NodeScopeAndName matmul = ParseNodeScopeAndName(node->name());
    const string optimized_node_name = OptimizedNodeName(matmul);
    if (ctx().node_map->NodeExists(optimized_node_name)) return Status::OK();

    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input));

    const NodeDef* transpose_op = node->op() == "Conj" ? input : node;
    const NodeDef* conj_op = node->op() == "Conj" ? node : input;

    if ((IsTranspose(*transpose_op) || IsConjugateTranspose(*transpose_op)) &&
        IsConj(*conj_op)) {
      NodeDef* new_op = AddCopyNode(optimized_node_name, transpose_op);

      // Flip the type of transpose op to absorb the conjugation.
      new_op->set_op(transpose_op->op() == "Transpose" ? "ConjugateTranspose"
                                                       : "Transpose");
      new_op->set_input(0, input->input(0));
      ctx().node_map->UpdateInput(new_op->name(), node->name(),
                                  input->input(0));
      ForwardControlDependencies(new_op, {node, input});
      *simplified_node_name = new_op->name();
    }

    return Status::OK();
  }