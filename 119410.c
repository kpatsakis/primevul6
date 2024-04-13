  bool IsSupported(const NodeDef* node) const override {
    return CanOptimize(*node) &&
           // Check that this node was not already a root of a fused chain. If
           // graph optimization runs twice without pruning in between,
           // fused_nodes_ will not have this information.
           !ctx().node_map->NodeExists(OptimizedNodeName(*node));
  }