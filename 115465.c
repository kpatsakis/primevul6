  void visit(Repetition &ope) override {
    ope.ope_->accept(*this);
    done_ = ope.min_ > 0;
  }