  void visit(PrioritizedChoice &ope) override {
    for (auto op : ope.opes_) {
      op->accept(*this);
      if (error_s) {
        done_ = true;
        break;
      }
    }
  }