  bool IsAbsorbableByOptimizedNodesGroup(const OptimizedNodesGroup& group,
                                         const NodeDef& node) const override {
    if (!CanOptimize(node)) return false;

    if (!IsOnTheSameDevice(group, node)) {
      return false;
    }
    // with a single output data consumer (presumably if we reach this node from
    // previously absorbed or a root node, it means that this node is not used
    // as an input to any other op, outside of the group)
    if (NumNonControlDataOutputs(node, *ctx().node_map) != 1) {
      return false;
    }
    // All input shapes must be broadcastable to the node shape
    const OpInfo::TensorProperties* properties;
    Status has_properties = GetTensorProperties(node.name(), &properties);
    return has_properties.ok() &&
           HasAllInputsBroadcastableToShape(node, *properties);
  }