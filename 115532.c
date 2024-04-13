  void visit(CaptureScope &ope) override {
    ope.ope_->accept(*this);
    found_ope = csc(found_ope);
  }