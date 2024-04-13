  void visit(AndPredicate &ope) override {
    ope.ope_->accept(*this);
    done_ = false;
  }