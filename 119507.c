  bool IsSupported(const NodeDef* node) const override {
    if (IsInPreserveSet(*node)) return false;
    if (IsConcat(*node) && node->attr().count("N") != 0) {
      const int n = node->attr().at("N").i();
      return n > 1 && FirstNInputsAreUnique(*node, n);
    } else if ((IsSplit(*node) || IsSplitV(*node)) &&
               node->attr().count("num_split") != 0) {
      const int num_split = node->attr().at("num_split").i();
      if (NumNonControlOutputs(*node, *ctx().node_map) > num_split) {
        // TODO(rmlarsen): Remove this constraint when we have optimizations
        // in place for merging slices into splits.
        return false;
      }
      if (NumControlOutputs(*node, *ctx().node_map) > 0) {
        // TODO(ezhulenev): Unary ops after Split might have a control path to
        // the Split node, and we currently do not properly handle cycles.
        return false;
      }
      return num_split > 1 && !IsAlreadyOptimized(*node);
    }
    return false;
  }