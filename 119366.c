  bool CanOptimize(const NodeDef& node) const {
    // TODO(ezhulenev): check if AccumulateNV2 can be supported too
    if (!IsAdd(node) && !IsAddN(node)) {
      return false;
    }
    if (IsInPreserveSet(node) || IsMarkedWithTag(node, kAddOpsRewriteTag)) {
      return false;
    }
    // TODO(ezhulenev): relax this condition for root node
    return !(IsDrivenByControlDependency(node) ||
             DrivesControlDependency(node));
  }