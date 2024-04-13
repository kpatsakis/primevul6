  void visit(PrecedenceClimbing &ope) override {
    ope.atom_->accept(*this);
    found_ope = csc(found_ope);
  }