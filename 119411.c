  std::size_t CountUniqueShapes(const std::vector<InputAndShape>& inputs) {
    std::set<string> sigs;
    for (const auto& ias : inputs) {
      sigs.insert(ShapeSignature(ias.shape));
    }
    return sigs.size();
  }