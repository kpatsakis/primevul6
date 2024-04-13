  void visit(CharacterClass &ope) override {
    found_ope = ope.shared_from_this();
  }