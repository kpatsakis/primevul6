  bool IsRewritten(const NodeDef* node) const {
    // if graph rewrite happens in multiple passes without graph pruning between
    // them, it's possible that rewritten node already exists in a graph
    return rewritten_nodes_.find(node->name()) != rewritten_nodes_.end() ||
           ctx().node_map->NodeExists(OuterNodeName(node, false)) ||
           ctx().node_map->NodeExists(OuterNodeName(node, true)) ||
           ctx().node_map->NodeExists(InnerAddNodeName(node));
  }