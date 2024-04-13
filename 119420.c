  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(EnsureNodeIsSupported(node));
    NodeDef* tail = node;
    tail = GetTailOfIdempotentChain(*tail, *ctx().node_map,
                                    *ctx().nodes_to_preserve);
    NodeDef* first_transpose;
    TF_RETURN_IF_ERROR(GetInputNode(tail->input(0), &first_transpose));

    NodeDef* node_perm;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &node_perm));
    if (!IsConstant(*node_perm)) {
      return Status::OK();
    }
    std::vector<int64> node_perm_values;
    TF_RETURN_IF_ERROR(GetPermutation(*node_perm, &node_perm_values));
    if (first_transpose->op() == node->op()) {
      // Remove pairs of transposes that cancel each other.
      NodeDef* first_transpose_perm;
      TF_RETURN_IF_ERROR(
          GetInputNode(first_transpose->input(1), &first_transpose_perm));
      if (!IsConstant(*first_transpose_perm)) {
        return Status::OK();
      }
      std::vector<int64> first_transpose_perm_values;
      TF_RETURN_IF_ERROR(
          GetPermutation(*first_transpose_perm, &first_transpose_perm_values));
      if (AreInversePermutations(node_perm_values,
                                 first_transpose_perm_values)) {
        if (tail == node) {
          // Bypass adjacent pair.
          *simplified_node_name = first_transpose->input(0);
        } else {
          // Bypass pair connected through chain.
          tail->set_input(0, first_transpose->input(0));
          ctx().node_map->UpdateInput(tail->name(), first_transpose->name(),
                                      first_transpose->input(0));
          ForwardControlDependencies(tail, {first_transpose});
          *simplified_node_name = node->input(0);
        }
      }
    } else {
      // Remove simple identity transposes.
      if (IsIdentityPermutation(node_perm_values)) {
        if (IsConjugateTranspose(*node)) {
          const NodeScopeAndName transpose =
              ParseNodeScopeAndName(node->name());
          const string optimized_node_name = OptimizedNodeName(transpose);
          NodeDef* new_op = AddCopyNode(optimized_node_name, node);
          new_op->set_op("Conj");
          new_op->mutable_input()->RemoveLast();
          new_op->mutable_attr()->erase("Tperm");
          ForwardControlDependencies(new_op, {node});
          *simplified_node_name = new_op->name();
        } else {
          *simplified_node_name = node->input(0);
        }
      }
    }
    return Status::OK();
  }