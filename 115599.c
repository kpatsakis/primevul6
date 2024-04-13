  void visit(Ignore &ope) override {
    ope.ope_->accept(*this);
    found_ope = ign(found_ope);
  }