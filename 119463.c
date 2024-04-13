  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(EnsureNodeIsSupported(node));

    OptimizedNodesGroup group;
    TF_RETURN_IF_ERROR(CreateOptimizedNodesGroup(node, &group));

    if (!group.optimized_nodes.empty()) {
      *simplified_node_name = RewriteOptimizedNodesGroup(group);
    }

    return Status::OK();
  }