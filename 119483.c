  bool IsMarkedWithAnyTag(const NodeDef& node, const StringPiece tag1,
                          const StringPiece tag2) const {
    return IsMarkedWithTag(node, tag1) || IsMarkedWithTag(node, tag2);
  }