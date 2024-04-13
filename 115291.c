  void visit(Repetition &ope) override {
    if (ope.max_ == std::numeric_limits<size_t>::max()) {
      HasEmptyElement vis(refs_);
      ope.ope_->accept(vis);
      if (vis.is_empty) {
        has_error = true;
        error_s = vis.error_s;
        error_name = vis.error_name;
      }
    } else {
      ope.ope_->accept(*this);
    }
  }