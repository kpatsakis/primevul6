  void visit(Whitespace &ope) override { ope.ope_->accept(*this); }