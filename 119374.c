  string FlipMinMax(const NodeDef& node) {
    const string& op = node.op();
    if (IsAnyMax(node) || IsArgMax(node)) {
      return str_util::StringReplace(op, "Max", "Min", false);
    } else {
      return str_util::StringReplace(op, "Min", "Max", false);
    }
  }