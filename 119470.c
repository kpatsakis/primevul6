bool AllRegularInputsEqual(const NodeDef& node) {
  if (!HasRegularInputs(node)) return true;
  for (int i = 1; i < node.input_size(); ++i) {
    if (IsControlInput(node.input(i))) {
      break;
    }
    if (node.input(0) != node.input(i)) {
      return false;
    }
  }
  return true;
}