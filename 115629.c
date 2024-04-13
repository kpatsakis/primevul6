  void visit(NotPredicate &ope) override {
    ope.ope_->accept(*this);
    found_ope = npd(found_ope);
  }