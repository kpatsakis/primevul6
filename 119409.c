  bool IsBinaryAssociative(const NodeDef& node) const {
    return IsMul(node) || IsAdd(node);
  }