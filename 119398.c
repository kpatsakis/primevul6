  bool IsMarkedWithTag(const NodeDef& node, const StringPiece tag) const {
    return HasNodeAttr(node, tag);
  }