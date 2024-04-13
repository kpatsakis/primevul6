  void visit(Sequence &ope) override {
    for (auto op : ope.opes_) {
      op->accept(*this);
    }
  }