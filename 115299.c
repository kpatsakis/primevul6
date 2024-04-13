  void visit(TokenBoundary &ope) override {
    ope.ope_->accept(*this);
    found_ope = tok(found_ope);
  }