  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* tail = GetTailOfValuePreservingChain(*node, *ctx().node_map,
                                                  *ctx().nodes_to_preserve);

    NodeDef* involution;
    TF_RETURN_IF_ERROR(GetInputNode(tail->input(0), &involution));

    if (involution->op() == node->op()) {
      // Skip both *node and *involution since they cancel each other.
      if (tail == node) {
        // The two nodes to eliminate are adjacent.
        *simplified_node_name = involution->input(0);
      } else {
        tail->set_input(0, involution->input(0));
        ctx().node_map->UpdateInput(tail->name(), involution->name(),
                                    involution->input(0));
        *simplified_node_name = node->input(0);
      }
    }

    return Status::OK();
  }