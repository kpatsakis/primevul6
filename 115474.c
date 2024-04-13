  void visit(Whitespace &ope) override {
    ope.ope_->accept(*this);
    found_ope = wsp(found_ope);
  }