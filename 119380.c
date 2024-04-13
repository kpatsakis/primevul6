  bool IsAbsorbableByOptimizedNodesGroup(const OptimizedNodesGroup& group,
                                         const NodeDef& node) const override {
    if (!IsSameOp(group, node)) {
      return false;
    }
    if (IsInPreserveSet(node)) {
      return false;
    }
    // Nodes optimized by AddOpsRewrite already have optimal broadcasts.
    if (IsMarkedWithAnyTag(node, kMinimizeBroadcastsTag, kAddOpsRewriteTag)) {
      return false;
    }
    if (IsDrivenByControlDependency(node) || DrivesControlDependency(node)) {
      return false;
    }
    if (!IsOnTheSameDevice(group, node)) {
      return false;
    }
    // Optimized nodes updated in place, and that would break the graph, if the
    // node has multiple output consumers
    if (NumNonControlOutputs(node, *ctx().node_map) != 1) {
      return false;
    }
    // All input shapes must be broadcastable to the node shape
    const OpInfo::TensorProperties* properties;
    Status has_properties = GetTensorProperties(node.name(), &properties);
    return has_properties.ok() &&
           HasAllInputsBroadcastableToShape(node, *properties);
  }