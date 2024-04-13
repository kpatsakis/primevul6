  void MarkWithTag(const StringPiece tag, NodeDef* node) {
    AddNodeAttr(tag, true, node);
  }