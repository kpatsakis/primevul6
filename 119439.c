  bool IsInnerMatrixTransposeNode(const NodeDef& transpose_node,
                                  const NodeMap* node_map) {
    if (transpose_node.op() != "Transpose" &&
        transpose_node.op() != "ConjugateTranspose") {
      return false;
    }
    const NodeDef* perm_node = node_map->GetNode(transpose_node.input(1));
    std::vector<int> perm32;
    if (ValuesFromConstNode(*perm_node, &perm32)) {
      return IsInnerMatrixTranspose(perm32);
    }
    std::vector<int64> perm64;
    if (ValuesFromConstNode(*perm_node, &perm64)) {
      return IsInnerMatrixTranspose(perm64);
    }
    return false;
  }