  bool IsAxis0(const NodeDef& node, int axis_input) {
    Tensor axis_tensor;
    if (!GetTensorFromConstNode(node.input(axis_input), &axis_tensor))
      return false;
    if (axis_tensor.NumElements() != 1) return false;
    if (axis_tensor.dtype() == DT_INT32) {
      return axis_tensor.flat<int32>()(0) == 0;
    } else if (axis_tensor.dtype() == DT_INT64) {
      return axis_tensor.flat<int64>()(0) == 0;
    } else {
      return false;
    }
  }