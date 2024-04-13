  void visit(AndPredicate &ope) override {
    ope.ope_->accept(*this);
    found_ope = apd(found_ope);
  }