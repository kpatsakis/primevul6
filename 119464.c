  bool OpsAreSafeToHoist(const NodeDef& root_node,
                         const ChainLinkSet& ops) const {
    if (ops.empty()) return true;
    const NodeDef* op0 = ops.begin()->node;
    if (ModifiesFrameInfo(*op0) || !IsUnaryElementWise(*op0)) return false;
    for (const auto& link : ops) {
      const NodeDef* op = link.node;
      if (op->device() != root_node.device() || op->op() != op0->op() ||
          IsInPreserveSet(*op)) {
        return false;
      }
      if (ctx().node_map->GetOutputs(op->name()).size() > 1) {
        // TODO(rmlarsen): Allow outgoing control edges.
        return false;
      }
      // Do not hoist Relu if it can be fused with its predecessors. This is
      // important because remapping runs after arithmetic.
      if (IsRelu(*op) || IsRelu6(*op)) {
        NodeDef* operand = nullptr;
        if (!GetInputNode(op->input(0), &operand).ok()) {
          return false;
        }
        if (IsFusedBatchNorm(*operand) || IsBiasAdd(*operand)) {
          return false;
        }
      }
    }
    return true;
  }