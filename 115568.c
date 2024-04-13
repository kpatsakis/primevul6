  void visit(NotPredicate &ope) override {
    ope.ope_->accept(*this);
    done_ = false;
  }