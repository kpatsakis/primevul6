  bool CanOptimize(const NodeDef& node) const {
    DataType dtype = GetDataTypeFromAttr(node, "T");
    if (!IsSupported(node.op(), dtype)) {
      return false;
    }
    if (IsInPreserveSet(node)) {
      return false;
    }
    if (!NodeIsOnCpu(node)) {
      return false;
    }
    if (NodeIsAlreadyFused(node)) {
      return false;
    }
    return !(IsDrivenByControlDependency(node) ||
             DrivesControlDependency(node));
  }